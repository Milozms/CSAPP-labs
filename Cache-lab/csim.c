/*
 Name : Maosen Zhang
 ID : 1500012898
*/
#include"cachelab.h"
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include<malloc.h>
#include<stdio.h>
typedef struct {
	long long tag;
	int v,recent;
}line;
int main(int argc, char** argv){
	char c;
	line** sets;
	int s, S, E, b, oc, hit_count=0, miss_count=0, eviction_count=0; 
	int tmp,i,exist,full,evict,count_opers=0,least_recent,target,empty_one;
	unsigned set_index;
	unsigned long long addr,tag;
	char *tracefile;
	while((oc=getopt(argc, argv, "s:E:b:t:"))!=-1){
		switch(oc){
			case's':
				s = atoi(optarg);
				break;
			case'E':
				E = atoi(optarg);
				break;
			case'b':
				b = atoi(optarg);
				break;
			case't':
				tracefile=optarg;
				break;
		}
	}
	S = pow(2,s);
	sets = (line**)malloc(sizeof(line*)*S);
	for (i = 0; i < S; ++i) {
		sets[i] = (line*)malloc(sizeof(line)*E);
		memset(sets[i], 0, sizeof(line)*E);
	}
	freopen(tracefile,"r",stdin);
	while(scanf("%c",&c)!=EOF){
		if(c=='I' || c=='L' || c=='M' || c=='S'){
			scanf("%llx,%d",&addr,&tmp);
			set_index = ((unsigned int)addr<<(32-s-b))>>(32-s);
			tag = addr>>(s+b);
			if(c!='I'){
				exist=empty_one=0;
				full=1;
				least_recent = count_opers+1;
				for(i=0;i<E;++i){
					if(sets[set_index][i].recent<least_recent){
						evict=i;
						least_recent=sets[set_index][i].recent;
					}
					if(sets[set_index][i].v && sets[set_index][i].tag==tag){
						exist=1;
						target = i;
					}
					if(!sets[set_index][i].v){
						full=0;
						if(!empty_one) empty_one=i;
					}
				}
				if(exist){
					hit_count++;
				}
				else if(!full){
					miss_count++;
					target=empty_one;
					sets[set_index][target].v = 1;
					sets[set_index][target].tag=tag;
				}
				else {
					miss_count++;
					eviction_count++;
					target=evict;
					sets[set_index][evict].tag=tag;
				}
				if(c=='M') hit_count++;
				sets[set_index][target].recent=count_opers;
				count_opers++;
			}
		}
	}
	printSummary(hit_count, miss_count, eviction_count);
	for (i = 0;i < S; ++i){
		free(sets[i]);
	}
	free(sets);
	return 0;
}