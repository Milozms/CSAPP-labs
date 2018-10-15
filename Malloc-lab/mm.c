/*
 * mm.c
 *
 * Name : Milozms
 * Segregate list allocator
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
//#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ 
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<9)  /* Extend heap by this amount (bytes) */  

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) 

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            
#define PUT(p, val)  (*(unsigned int *)(p) = (unsigned int)(val))    

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   
#define GET_ALLOC(p) (GET(p) & 0x1)                    

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) 

#define SET_NEXT_ALLOC(bp) \
 PUT(HDRP(NEXT_BLKP(bp)), GET(HDRP(NEXT_BLKP(bp))) | 0X2)
#define SET_NEXT_UNALLOC(bp) \
 PUT(HDRP(NEXT_BLKP(bp)), GET(HDRP(NEXT_BLKP(bp))) & ~0x2)
#define GET_PREV_ALLOC(bp) ((GET(HDRP(bp)) & 0x2)>>1)
#define SET_PREV_ALLOC(bp, alloc) PUT(HDRP(bp), GET(HDRP(bp)) | (alloc<<1)) 

 /* Given block ptr bp, 
 compute address of prev/next ptr (to prev/next FREE block) */
#define PREV_PTR(bp)  (char *)(bp + WSIZE)
#define NEXT_PTR(bp)  (char *)(bp)
 /* Given block ptr bp, compute address of prev/next free block */
#define PREV_FREE(bp)  (char *)(GET(PREV_PTR(bp)) + start_of_heap)
#define NEXT_FREE(bp)  (char *)(GET(NEXT_PTR(bp)) + start_of_heap)
#define ADR_CAST(bp)   ((unsigned int)bp - (unsigned int)start_of_heap)
#define ADR_RECV(bp)   (char *)(bp + start_of_heap)


/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */  
static char *start_of_heap = 0;
static char *list_heads_end = 0;
static char *first_head = 0;
/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);

/*the next_ptr of last block in the list point 0*/
/*select the list for the size*/
void *select_list(size_t size){
	/*size >= 4096*/
	if(size>>12){
		return (void*)(ADR_RECV(WSIZE*13));
	}
	if(size>>11){
		return (void*)(ADR_RECV(WSIZE*12));
	}
	if(size>>10){
		return (void*)(ADR_RECV(WSIZE*11));
	}
	if(size>>9){
		return (void*)(ADR_RECV(WSIZE*10));
	}
	if(size>>8){
		return (void*)(ADR_RECV(WSIZE*9));
	}
	if(size>>7){
		return (void*)(ADR_RECV(WSIZE*8));
	}
	if(size>>6){
		return (void*)(ADR_RECV(WSIZE*7));
	}
	if(size>>5){
		return (void*)(ADR_RECV(WSIZE*6));
	}
	if(size>>4){
		return (void*)(ADR_RECV(WSIZE*5));
	}
	if(size>>3){
		return (void*)(ADR_RECV(WSIZE*4));
	}
	if(size>>2){
		return (void*)(ADR_RECV(WSIZE*3));
	}
	if(size>>1){
		return (void*)(ADR_RECV(WSIZE*2));
	}
	return (void*)(ADR_RECV(WSIZE));
}

/*insert block into list, LIFO*/
void insert_block(void *head, void *bp){
	void* next = NEXT_FREE(head);
	PUT(NEXT_PTR(bp), GET(head));
	PUT(PREV_PTR(bp), head);
	PUT(NEXT_PTR(head), ADR_CAST(bp));
	if(next != start_of_heap) 
		PUT(PREV_PTR(next), ADR_CAST(bp));
}
/*find free block*/
void *find_block(void *head, size_t size){
	void *bp = ADR_RECV(GET(head));
	while(bp != start_of_heap){
		if(size <= GET_SIZE(HDRP(bp))){
			return bp;
		}
		bp = NEXT_FREE(bp);
	}
	return NULL;
}
/*delete free block*/
void delete_block(void* bp){
	void *prev = PREV_FREE(bp), *next = NEXT_FREE(bp);
	PUT(NEXT_PTR(prev), next);
	/*if bp is not last block*/
	if(next != start_of_heap){
		PUT(PREV_PTR(next), prev);
	}
}
/* 
 * mm_init - Initialize the memory manager 
 */
int mm_init(void) 
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(18*WSIZE)) == (void *)-1) 
        return -1;
    start_of_heap = heap_listp;
    first_head = heap_listp + WSIZE;
    list_heads_end = (void*)(ADR_RECV(WSIZE*14));
    memset(start_of_heap, 0, WSIZE*15);

    heap_listp += 16*WSIZE;
    PUT(HDRP(heap_listp), PACK(2*WSIZE, 1));            /* Prologue header */ 
    PUT(FTRP(heap_listp), PACK(2*WSIZE, 1));            /* Prologue footer */
    PUT(HDRP(heap_listp + 2*WSIZE), PACK(0, 1)); 	    /* Epilogue header */
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;
    return 0;
}

/* 
 * malloc - Allocate a block with at least size bytes of payload 
 */
void *malloc(size_t size) 
{
	size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;      

    if (heap_listp == 0){
        mm_init();
    }
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)                                          
        asize = 2*DSIZE;                                        
    else
        asize = DSIZE * ((size + (WSIZE) + (DSIZE-1)) / DSIZE); 
    asize = MAX(asize, 4*WSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {  
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);                 
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)  
        return NULL;                                  
    place(bp, asize);  
    SET_NEXT_ALLOC(bp);                         
    return bp;
} 

/* 
 * free - Free a block 
 */
void free(void *bp)
{
    if (bp == 0) 
        return;
    if (heap_listp == 0){
        mm_init();
    }
    size_t size = GET_SIZE(HDRP(bp)), vsize;
    size_t prev_alloc = GET_PREV_ALLOC(bp);
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    SET_PREV_ALLOC(bp, prev_alloc);
    SET_NEXT_ALLOC(heap_listp);
    bp = coalesce(bp);
    size = GET_SIZE(HDRP(bp));
    vsize = size - DSIZE;
    char *head = select_list(vsize);    
    insert_block(head, bp);
    SET_NEXT_UNALLOC(bp);
}

/*
 * realloc - Naive implementation of realloc
 */
void *realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(ptr == NULL) {
        return mm_malloc(size);
    }

    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if(!newptr) {
        return 0;
    }

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    if(size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}

/* 
 * mm_checkheap - Check the heap for correctness. Helpful hint: You
 *                can call this function using mm_checkheap(__LINE__);
 *                to identify the line number of the call site.
 */
void mm_checkheap(int lineno)  
{ 
	char* head;
	int i;
	for(head = first_head, i = 1; 
        head<=list_heads_end; head += WSIZE, i++){
	    char* ptr = head;
	    if(GET(head) != 0){
		    while(ptr != start_of_heap){
		    	if(ptr != head && GET_ALLOC(HDRP(ptr))){
		    		exit(0);
		    	}
		    	if(NEXT_FREE(ptr) != start_of_heap 
                    && ptr != PREV_FREE(NEXT_FREE(ptr))){
		    		exit(0);
		    	}
		    	ptr = NEXT_FREE(ptr);
		    }
		}
	}
}

/* 
 * The remaining routines are internal helper routines 
 */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
 /*called by mm_init and malloc*/
static void *extend_heap(size_t words) 
{
    char *bp, *head;
    size_t size, vsize;
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; 
    size = MAX(size, 4*WSIZE);
    vsize = size - DSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)  
        return NULL;                                        
    
    /* Initialize free block header/footer and the epilogue header */
    size_t prev_alloc = GET_PREV_ALLOC(bp);
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */ 
    PUT(FTRP(bp), PACK(size, 0));
    SET_PREV_ALLOC(bp, prev_alloc);
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */
    SET_NEXT_ALLOC(heap_listp);
    bp = coalesce(bp);                                          
    vsize = GET_SIZE(HDRP(bp)) - DSIZE;
   	head = select_list(vsize);
   	insert_block(head, bp);
    return bp;
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
 /*called by extend_heap and free*/
static void *coalesce(void *bp) 
{
    char* prev = PREV_BLKP(bp);
    char* next = NEXT_BLKP(bp);
    size_t prev_alloc = GET_PREV_ALLOC(bp);
    size_t next_alloc = GET_ALLOC(HDRP(next));
    size_t size = GET_SIZE(HDRP(bp));
    if (prev_alloc && next_alloc) {            /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
    	delete_block(next);
        size += GET_SIZE(HDRP(next));
        size_t prev_alloc = GET_PREV_ALLOC(bp);
	    PUT(HDRP(bp), PACK(size, 0));
	    PUT(FTRP(bp), PACK(size, 0));
	    SET_PREV_ALLOC(bp, prev_alloc);
        
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
    	delete_block(prev);
        size += GET_SIZE(HDRP(prev));
        PUT(FTRP(bp), PACK(size, 0));
        size_t prev_prev_alloc = GET_PREV_ALLOC(prev);
        PUT(HDRP(prev), PACK(size, 0));
        SET_PREV_ALLOC(prev, prev_prev_alloc);
        bp = prev;
    }

    else {                                     /* Case 4 */
	    delete_block(next);
    	delete_block(prev);
        size += GET_SIZE(HDRP(prev)) + 
            GET_SIZE(FTRP(next));
        size_t prev_prev_alloc = GET_PREV_ALLOC(prev);
        PUT(HDRP(prev), PACK(size, 0));
        SET_PREV_ALLOC(prev, prev_prev_alloc);
        PUT(FTRP(next), PACK(size, 0));
        bp = prev;
    }
    SET_NEXT_ALLOC(heap_listp);
    //head = select_list(size);
    //insert_block(head, bp);
    return bp;
}

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
 /*called by malloc*/
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp)), vsize;
    void *newbp, *head;
    delete_block(bp);  
    if ((csize - asize) >= (2*DSIZE)) { 
        size_t prev_alloc = GET_PREV_ALLOC(bp);
	    PUT(HDRP(bp), PACK(asize, 1));
	    PUT(FTRP(bp), PACK(asize, 1));
	    SET_PREV_ALLOC(bp, prev_alloc);
        newbp = NEXT_BLKP(bp);
        PUT(HDRP(newbp), PACK(csize-asize, 0));
        PUT(FTRP(newbp), PACK(csize-asize, 0));
        SET_NEXT_ALLOC(bp);
        vsize = csize - asize - DSIZE;
        head = select_list(vsize);
        insert_block(head, newbp);
    }
    else { 
        size_t prev_alloc = GET_PREV_ALLOC(bp);
	    PUT(HDRP(bp), PACK(csize, 1));
	    PUT(FTRP(bp), PACK(csize, 1));
	    SET_PREV_ALLOC(bp, prev_alloc);
	    SET_NEXT_ALLOC(bp);
    }
    SET_NEXT_ALLOC(heap_listp);
}

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
/*called by malloc*/
static void *find_fit(size_t asize)
{
    /* First-fit search */
    void *bp, *head;
    for(head = select_list(asize); 
        head <= (void*)list_heads_end; head += WSIZE){
    	bp = find_block(head, asize);
    	if(bp){
    		return bp;
    	}
    }
    return NULL; /* No fit */
}

