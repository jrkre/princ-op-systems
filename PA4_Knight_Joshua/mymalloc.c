#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include "print_memlist.c"


//commented out the struct to avoid redefinition from print_memlist.c
// typedef struct _mblock_t {
//     struct _mblock_t *prev;
//     struct _mblock_t *next;
//     size_t size;
//     int status;  // 1: Allocated, 0: Free
//     void *payload; // Points to the actual data for this block
// } mblock_t;

// typedef struct _mlist_t {
//     mblock_t *head;
// } mlist_t;

#define MBLOCK_HEADER_SZ offsetof(mblock_t, payload)

mlist_t mlist = { NULL };  // global mem list to NULL

mblock_t * findLastMemlistBlock()
{
    mblock_t *head = mlist.head;

    
    while (head != NULL && head->next != NULL) {
        head = head->next;
    }

    return head;
}

mblock_t * findFreeBlockOfSize(size_t size)
{
    mblock_t *head = mlist.head;
    while (head != NULL) {
        if (head->status == 0 && head->size >= size) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void splitBlockAtSize(mblock_t * block, size_t newSize)
{

    mblock_t *new_block = (mblock_t *) ((char *)block + MBLOCK_HEADER_SZ + newSize);

    // set up the new block, which is the remainder of the old block after split
    new_block->size = block->size - newSize - MBLOCK_HEADER_SZ;
    new_block->status = 0;
    new_block->next = block->next;
    new_block->prev = block;

    block->size = newSize;
    block->status = 1;
    block->next = new_block;

    if (new_block->next) {
        new_block->next->prev = new_block;
    }
}

void coallesceBlockPrev(mblock_t * block)
{
    if (block->prev && block->prev->status == 0) {
        block->prev->size += block->size + MBLOCK_HEADER_SZ;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
        block->prev = NULL;
    }
}

void coallesceBlockNext(mblock_t * block)
{
    if (block->next != NULL && block->next->status == 0) {
        block->size += block->next->size + MBLOCK_HEADER_SZ;
        block->next = block->next->next;
        if (block->next != NULL) {
            block->next->prev = block;
        }
    }
}

mblock_t * growHeapBySize(size_t size)
{

    mblock_t *newBlock = (mblock_t *) sbrk(size + sizeof(mblock_t));

    if (newBlock == (void *)-1) { // thats a weird fkn error val uwu
        return NULL;
    }

    newBlock->size = size;
    newBlock->status = 1;
    newBlock->prev = findLastMemlistBlock();
    newBlock->next = NULL;
    newBlock->payload = (char *)newBlock + sizeof(mblock_t);

    if (mlist.head == NULL) {
        mlist.head = newBlock;
    } else {
        mblock_t *lastBlock = findLastMemlistBlock();
        lastBlock->next = newBlock;
    }

    return newBlock;
}


void* mymalloc(size_t size)
{
    if (size <= 0) {
        return NULL;
    }

    //find block
    mblock_t *freeBlock = findFreeBlockOfSize(size);

    // if free block is found, split it if necessary and return the memory
    if (freeBlock != NULL) {
        splitBlockAtSize(freeBlock, size);
        return freeBlock->payload;
    }
    
    // no free block, grow the heap and return the new block
    mblock_t *newBlock = growHeapBySize(size);
    // sbrk failed -> its joever
    if (!newBlock) {
        return NULL;
    }

    newBlock->status = 1;
    newBlock->size = size;
    newBlock->payload = (char *)newBlock + MBLOCK_HEADER_SZ;
    
    return newBlock->payload;
}

void myfree(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    mblock_t *block = (mblock_t *)((char *)ptr - MBLOCK_HEADER_SZ);
    block->status = 0; // mark the block as free

    coallesceBlockPrev(block);
    coallesceBlockNext(block);

    // if the block is the only block in the list, set the head to next
    if (mlist.head == block && block->next == NULL) {
        mlist.head = NULL;
    } else if (block->prev != NULL) {
        block->prev->next = block->next;
    }
    if (mlist.head == block) {
        mlist.head = block->next;
    }

    
}

int main(int argc, char *argv[]) 
{

    void *p1 = mymalloc(10);
    void *p2 = mymalloc(100);
    void *p3 = mymalloc(200);
    void *p4 = mymalloc(500);

    printf("\nfirst - allocation\n");
    printMemList(mlist.head);

    myfree(p3); p3 = NULL;
    myfree(p2); p2 = NULL;

    printf("\nsecond - deallocation\n\n");
    printMemList(mlist.head);

    void *p5 = mymalloc(150);
    void *p6 = mymalloc(500);
    
    printf("\nthird - allocation\n\n");
    printMemList(mlist.head);

    myfree(p4); p4 = NULL;
    myfree(p5); p5 = NULL;
    myfree(p6); p6 = NULL;
    myfree(p1); p1 = NULL;

    printf("\nfourth - final deallocation\n\n");
    printMemList(mlist.head);

    return 0;
    
}
