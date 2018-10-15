#include "csapp.h"
//#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_OBJECT_NUM 10
typedef struct{
    int connfd;
    int count;
} connarg;
typedef struct{
	char content[MAX_OBJECT_SIZE];
    int len;
	char url[MAXLINE];
	int recent;//recent = -1: invalid
    sem_t mutex, w;
    sem_t url_mutex, url_w;
    sem_t recent_mutex, recent_w;
    int readcnt, url_readcnt, recent_readcnt;
} cache_object;
cache_object cache[MAX_OBJECT_NUM];
void *thread(void* vargp);
void doit(int fd, int count);
int parse_url(const char* url, char* hostname, char* uri, char* port);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
int find_url(const char* url);
int find_evict();
void read_from_cache(int fd, int i, int count);
void server_request(const char* url, int clientfd, int fd, int count);
void init_cache();
void url_reader(int i, char* dst);
int recent_reader(int i);
void content_reader(int i, int fd);
void url_writer(int i, const char* val);
void recent_writer(int i, int val);
void content_writer(int i, const char* val, int len);
/* You won't lose style points for including this long line in your code */
static char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static char *conn_hdr = "Connection: close\r\n";
static char *proxy_conn_hdr = "Proxy-Connection: close\r\n";

int main(int argc, char **argv) 
{
    int listenfd;
    connarg *conn;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    printf("%s", user_agent_hdr);
    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    Signal(SIGPIPE, SIG_IGN);
    listenfd = Open_listenfd(argv[1]);
    int count = 1;
    init_cache();
    while (1) {
		clientlen = sizeof(clientaddr);
		conn = (connarg*)Malloc(sizeof(connarg));
		conn->connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); 
        conn->count = count;
        count++;
	    Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
	                    port, MAXLINE, 0);
	    dbg_printf("Accepted connection from (%s, %s)\n", hostname, port);
		Pthread_create(&tid, NULL, thread, conn);
		//doit(*connfdp);
    }
}
/* $end main */

/*Thread routine*/
void *thread(void* vargp){
	connarg *conn = (connarg*)vargp;
    int fd = conn->connfd;
    int count = conn->count;
	Pthread_detach(pthread_self());
    Free(vargp);
    doit(fd, count);
    Close(fd);
    return NULL;
}

/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int fd, int count) 
{
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], client_hdr[MAXLINE],
    hostname[MAXLINE], uri[MAXLINE], version[MAXLINE], port[MAXLINE];
    rio_t connrio;
    int cache_index;
    /* Read request line and headers */
    rio_readinitb(&connrio, fd);
    if (!rio_readlineb(&connrio, buf, MAXLINE))
        return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, url, version);

	if(sscanf(buf, "%s %s %s", method, url, version) < 3){  
        fprintf(stderr, "sscanf error\n");  
        clienterror(fd, method, "404","Not Found", "Not Found");    
        Close(fd);  
        return;  
    }

    if (strcasecmp(method, "GET")) {
    	fprintf(stderr, "method error\n");  
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }

    //parse url
    if(parse_url(url, hostname, uri, port) < 0){
    	fprintf(stderr, "url error\n");    
        Close(fd);  
        return;
    }

    //deal with client-sent headers
    rio_readlineb(&connrio, client_hdr, MAXLINE);
    //rio_writen(clientfd, buf, strlen(buf));
    while(strcmp(client_hdr, "\r\n")) {
        rio_readlineb(&connrio, client_hdr, MAXLINE);
        //rio_writen(clientfd, buf, strlen(buf));
    }

    //if cached: read from cache
    if((cache_index = find_url(url)) >= 0){
        content_reader(cache_index, fd);
    	return;
    }
    //connect to server
    dbg_printf("breakpoint 1 : %s %s\r\n", hostname, port);
    int clientfd = open_clientfd(hostname, port);
    if(clientfd < 0){
    	fprintf(stderr, "connect to server error\n");    
        Close(fd);  
        return;
    }
    //HTTP GET request
    sprintf(buf, "GET %s HTTP/1.0\r\n", uri);
    rio_writen(clientfd, buf, strlen(buf));
    //request headers
    sprintf(buf, "Host: %s\r\n", hostname);
    rio_writen(clientfd, buf, strlen(buf));
    rio_writen(clientfd, user_agent_hdr, strlen(user_agent_hdr));
    rio_writen(clientfd, conn_hdr, strlen(conn_hdr));
    rio_writen(clientfd, proxy_conn_hdr, strlen(proxy_conn_hdr));
    rio_writen(clientfd, "\r\n", strlen("\r\n"));
    printf("request to server is done.\n");

    //read from clientfd    
    server_request(url, clientfd, fd, count);
    Close(clientfd);
}
/* $end doit */

/*
 *parse_url - split url into hostname, port and uri
 */
int parse_url(const char* url, char* hostname, char* uri, char* port){
	char tmpurl[MAXLINE];
	char *prefixstart, *uristart, *portstart;
	strcpy(tmpurl, url);
	prefixstart = index(tmpurl, '/');//first'/'
	if(prefixstart == NULL)
		return -1;
	prefixstart += 2;//second'/'
	uristart = index(prefixstart,'/');
	if(uristart == NULL)
		return -1;
	strcpy(uri, uristart);
	*uristart = '\0';
	portstart = index(prefixstart, ':');
	if(portstart == NULL){
		strcpy(hostname, prefixstart);
		strcpy(port, "80"); // default port
	}
	else{
		*portstart = '\0';
		portstart++;
		strcpy(port, portstart);
		strcpy(hostname, prefixstart);
	}
	return 0;
}

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
/* $end clienterror */

/*
 * find_url: find if this object has been cached
 */
int find_url(const char* url){
    char tmpurl[MAXLINE];
    int tmprecent, i;
	for(i=0;i<MAX_OBJECT_NUM;++i){
        url_reader(i, tmpurl);
        tmprecent = recent_reader(i);
		if(tmprecent > 0 && strcmp(url, tmpurl) == 0){
			return i;
		}
	}
	return -1;
}

/*
 * find_evict: find the LRU to evict
 */
int find_evict(){
	int minrecent = 1<<30, evict = 0, 
    countvalid = 0, i = 0, tmprecent, firstempty = -1;
	for(i=0;i<MAX_OBJECT_NUM;++i){
        tmprecent = recent_reader(i);
        if(tmprecent > 0){
            countvalid++;
        }
        if(tmprecent == -1 && firstempty == -1){
            firstempty = i;
        }
		if(tmprecent < minrecent){
			evict = i;
			minrecent = tmprecent;
		}
	}
	if(countvalid < MAX_OBJECT_NUM) 
        return firstempty;
    return evict;
}

void server_request(const char* url, int clientfd, int fd, int count){
    char buf[MAXLINE];
	char tmpcontent[MAX_OBJECT_SIZE];
	int len = 0, n, i;
    rio_t clientrio;
    char *p = tmpcontent;
	//read from clientfd
    rio_readinitb(&clientrio, clientfd);
    while((n = rio_readnb(&clientrio, buf, MAXLINE)) > 0){
        rio_writen(fd, buf, n);
        if(len + n < MAX_OBJECT_SIZE){
            strncpy(p, buf, n);
            p += n;
        }
        len += n;
    }
    if(len < MAX_OBJECT_SIZE){
        i = find_evict();
        //writer        
        url_writer(i, url);
        recent_writer(i, count);
        content_writer(i, tmpcontent, len);
    }
}

void init_cache(){
    int i;
    for(i=0;i<MAX_OBJECT_NUM;++i){
        Sem_init(&cache[i].mutex, 0, 1);
        Sem_init(&cache[i].w, 0, 1);
        Sem_init(&cache[i].url_mutex, 0, 1);
        Sem_init(&cache[i].url_w, 0, 1);
        Sem_init(&cache[i].recent_mutex, 0, 1);
        Sem_init(&cache[i].recent_w, 0, 1);
        cache[i].readcnt = cache[i].url_readcnt = cache[i].recent_readcnt = 0;
        cache[i].recent = -1;
    }
}

void url_reader(int i, char* dst){
    P(&cache[i].url_mutex);
    cache[i].url_readcnt++;
    if(cache[i].url_readcnt == 1){
        P(&cache[i].url_w);
    }
    V(&cache[i].url_mutex);
    //reading happens
    strcpy(dst, cache[i].url);
    //reading ends
    P(&cache[i].url_mutex);
    cache[i].url_readcnt--;
    if(cache[i].url_readcnt == 0){
        V(&cache[i].url_w);
    }
    V(&cache[i].url_mutex);
}

int recent_reader(int i){
    int recent;
    P(&cache[i].recent_mutex);
    cache[i].recent_readcnt++;
    if(cache[i].recent_readcnt == 1){
        P(&cache[i].recent_w);
    }
    V(&cache[i].recent_mutex);
    //reading happens
    recent = cache[i].recent;
    //reading ends
    P(&cache[i].recent_mutex);
    cache[i].recent_readcnt--;
    if(cache[i].recent_readcnt == 0){
        V(&cache[i].recent_w);
    }
    V(&cache[i].recent_mutex);
    return recent;
}

void content_reader(int i, int fd){
    P(&cache[i].mutex);
    cache[i].readcnt++;
    if(cache[i].readcnt == 1){
        P(&cache[i].w);
    }
    V(&cache[i].mutex);
    //reading happens
    rio_writen(fd, cache[i].content, cache[i].len);
    //reading ends
    P(&cache[i].mutex);
    cache[i].readcnt--;
    if(cache[i].readcnt == 0){
        V(&cache[i].w);
    }
    V(&cache[i].mutex);
}

void url_writer(int i, const char* val){
    P(&cache[i].url_w);
    strcpy(cache[i].url, val);
    V(&cache[i].url_w);
}

void recent_writer(int i, int val){
    P(&cache[i].recent_w);
    cache[i].recent = val;
    V(&cache[i].recent_w);
}

void content_writer(int i, const char* val, int len){
    P(&cache[i].w);
    strncpy(cache[i].content, val, len);
    cache[i].len = len;
    V(&cache[i].w);
}