
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE       4
#define DSIZE       8
#define CHUNKSIZE   (1<<12)

#define MAX(x,y)    ((x)>(y)?(x):(y))

#define PACK(size,alloc) ((size)|(alloc))

#define GET(p)      (*(unsigned int*)(p))
#define PUT(p,val)  (*(unsigned int*)(p)=(val)) //合并大小和已分配位

#define GET_SIZE(p) (GET(p)&~0x7) //返回大小
#define GET_ALLOC(p) (GET(p)&0x1) //返回已分配位

#define HDRP(bp)    ((char*)(bp)-WSIZE) //由块指针得到块的头部
#define FTRP(bp)    ((char*)(bp)+GET_SIZE(HDRP(bp))-DSIZE)  //由块指针得到块的尾部

#define NEXT_BLKP(bp) ((char*)(bp)+GET_SIZE(((char*)(bp)-WSIZE))) //指向后面的块的指针
#define PREV_BLKP(bp) ((char*)(bp)-GET_SIZE(((char*)(bp)-DSIZE))) //指向前面的块的指针

#define PRE_BLANK_P ((char*)(bp))
#define NEXT_BLANK_P(bp) ((char*)((bp)+WSIZE))
#define NEXT_BLANK(bp) (GET((char*)((bp)+WSIZE)))
#define PRE_BLANK(bp)  (GET((char*)(bp)))
/* 
 * mm_init - initialize the malloc package.
 */

static char* heap_listp;    //指向序言块的脚部位置
static char* head;
static char* tail;


/*=========================声明========================*/
static void *extend_heap(size_t);
static void *coalesce(void *);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);



/*extend_heap 扩展堆，双字对齐*/
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    /*分配偶数字，为了对齐*/
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if((long) (bp = mem_sbrk(size)) == -1) return NULL;
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));
    
    PUT(bp+WSIZE,NEXT_BLANK(head));
    if(NEXT_BLANK(head)) PUT(NEXT_BLANK(head),bp);
    PUT(bp,head);
    PUT(head+WSIZE,bp);
    
    

    return coalesce(bp);
}
/*coalesce 带边界标记的合并*/
static void *coalesce(void *bp)
{
    // char* foot=FTRP(PREV_BLKP(bp));
    // char* next_bp=NEXT_BLKP(bp);
    // char* bphead=HDRP(NEXT_BLKP(bp));
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    if(prev_alloc && next_alloc) return bp;
    else if (prev_alloc && !next_alloc) 
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size,0));
        PUT(FTRP(bp), PACK(size,0));

        PUT(PRE_BLANK(NEXT_BLKP(bp))+WSIZE,NEXT_BLANK(NEXT_BLKP(bp)));
        PUT(NEXT_BLANK(NEXT_BLKP(bp)),PRE_BLANK(NEXT_BLANK(bp)));

        
    }
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));

        PUT(PRE_BLANK(bp)+WSIZE,NEXT_BLANK(bp));
        if(NEXT_BLANK(bp))PUT(NEXT_BLANK(bp),PRE_BLANK(bp));
        
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));

        PUT(PRE_BLANK(bp)+WSIZE,NEXT_BLANK(bp));
        if(NEXT_BLANK(bp))PUT(NEXT_BLANK(bp),PRE_BLANK(bp));
        PUT(PRE_BLANK(NEXT_BLKP(bp))+WSIZE,NEXT_BLANK(NEXT_BLKP(bp)));
        PUT(NEXT_BLANK(NEXT_BLKP(bp)),PRE_BLANK(NEXT_BLANK(bp)));

        bp = PREV_BLKP(bp);
    }
    return bp;
}

//最佳适配
static void *find_fit(size_t asize){
    void*bp;
    void*best=NULL;
    int min=1<<30;
    for(bp=head;bp!=NULL;bp=NEXT_BLANK(bp)){
        if((asize<=GET_SIZE(HDRP(bp)))){
            if(GET_SIZE(HDRP(bp))-asize<=min){
                min=GET_SIZE(HDRP(bp))-asize;
                best=bp;
            }
        }
    }
    return best;
}

static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    if((csize - asize) >= (2*DSIZE))
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));

        PUT(PRE_BLANK(bp)+WSIZE,NEXT_BLKP(bp));
        if(NEXT_BLANK(bp))PUT(NEXT_BLANK(bp),NEXT_BLKP(bp));
        PUT(NEXT_BLKP(bp),PRE_BLANK(bp));
        PUT(NEXT_BLKP(bp)+WSIZE,NEXT_BLANK(bp));

        

        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1)); 

        PUT(PRE_BLANK(bp)+WSIZE,NEXT_BLANK(bp));
        if(NEXT_BLANK(bp)) PUT(NEXT_BLANK(bp),PRE_BLANK(bp)); 

            
    }
}

int mm_init(void)
{
    if((heap_listp=mem_sbrk(6*WSIZE))==(void*)-1)
        return -1;
    PUT(heap_listp,0);
    PUT(heap_listp+WSIZE,0);
    PUT(heap_listp+(1*WSIZE),PACK(4*WSIZE,1));
    PUT(heap_listp+(4*WSIZE),PACK(4*WSIZE,1));

    head=heap_listp+(DSIZE);
    

    PUT(heap_listp+(5*WSIZE),PACK(0,1));
    heap_listp+=(4*WSIZE);

    char* temp;
    if((temp=extend_heap(CHUNKSIZE/WSIZE))==NULL)
        return -1;

    PUT(head,NULL);
    PUT(head+WSIZE,temp);
    
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;
    if(size == 0) return NULL;
    if(size <= DSIZE) asize = 2*DSIZE;
    else asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    if((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize/WSIZE)) == NULL) return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size=GET_SIZE(HDRP(bp));
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));

    PUT(bp,head);
    PUT(bp+WSIZE,NEXT_BLANK(head));
    if(NEXT_BLANK(head)) PUT(NEXT_BLANK(head),bp);
    PUT(head+WSIZE,bp);
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
    
}














