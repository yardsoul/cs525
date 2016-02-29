#include "buffer_mgr.h"

typedef struct FrameInfo
{
	// Pointer to the data of the frame
	char * frameData;
	// Frame Number in the Buffer Pool (from 0 to numPages-1)
	int frameNumber;
	// Number of the page in the page file
	PageNumber pageNumber;
	// Counts how many users are reading the frame
	int fixCount;
	// Boolean that gives information about changes in the file
	bool isDirty;
	// Previous Frame in the Buffer Pool
	struct FrameInfo *previousFrame;
	// Next Frame in the Buffer Pool
	struct FrameInfo *nextFrame;
} FrameInfo;

typedef struct BufferPoolInfo
{
	// Pointer to the Buffer Pool
	FrameInfo *bufferPool;
	// First frame in the Buffer Pool
	FrameInfo *firstFrame;
	// Last frame in the Buffer Pool
	FrameInfo *lastFrame;
	// Number of pages read
	int readNumber;
	// Number of pages written
	int writeNumber;
} BufferPoolInfo;

/************************************************************
 *                 Replacement Strategies                   *
 ************************************************************/

// FIFO
//2016-02-27 Pat
RC doFifo(BM_BufferPool *const bm, BM_PageHandle *const page,PageNumber pageNum)
{
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	if(buffPoolInfo->firstFrame->nextFrame!=NULL)
	{
		//If dirty write it down to disk
		if(buffPoolInfo->firstFrame->isDirty)
		{
			RC writeDirty = forcePage(bm,buffPoolInfo->firstFrame->pageNumber);
			if(writeDirty != RC_OK)
			{
				return RC_WRITE_FAILED;
			}
		}
		//Pop first page
		buffPoolInfo->firstFrame = &buffPoolInfo->firstFrame->nextFrame;
		buffPoolInfo->firstFrame->previousFrame = NULL;
		
		//Read page to memory
		RC readToMem = readBlock(pageNum, bm->mgmtData, buffPoolInfo->lastFrame->nextFrame->frameData);
		if(readToMem != RC_OK)
		{
			return RC_READ_NON_EXISTING_PAGE;
		}
		buffPoolInfo->readNumber++;
		buffPoolInfo->lastFrame = &buffPoolInfo->lastFrame->nextFrame;
		buffPoolInfo->lastFrame->pageNumber = pageNum;
		page->data = buffPoolInfo->lastFrame->frameData
		return RC_OK;
	}
}
// LRU
typedef struct Node
{
    //pointer for Doubly linked List
    struct Node *prev;
    //pointer for Doubly Linked list
    struct Node *next;
    //the frame number stored in this QNode
    int frameNumber
}

typedef struct struct Queue
{
    //count the number of filled frames
    int count;
    //total number of frames
    int numberFrames;
    //first node in Doubly Linked List
    Node *head;
    //last node in Doubly Linked List
    Node *tail;
}

typedef struct Hash
{
    //number of frames that can be stored
    int capacity
    //an array of nodes
    Node* *array
}

Node* createNode(int frameNumber)
{
    //allocate memory 
    Node* temp = (Node *)malloc( sizeof(Node));
    //assign the given frameNumner to the temp Node
    temp -> frameNumber = frameNumber;
    //initialize prev and next pointer as NULL
    temp->prev= temp->next=NULL;
    return temp;
}

Queue* createQueue( int numberFrames)
{
    //allocate memory
    Queue* queue = (Queue *)malloc(sizeof(Queue));
    //set the count to 0
    queue->count = 0;
    //assign the head and tail as NULL
    queue-> head = queue->tail= NULL;
    
    //number of frames that can be stored
    queue->numberFrames = numberFrames;
}

Hash* createHash(int capacity)
{
    //allocate memory
    Hash* hash =(Hash *)malloc(sizeof(Hash));
    //assign the capacity
    hash-> capacity = capacity;
    //create an array of index
    hash-> array =(Node**)malloc(hash->capacity * sizeof(Node));
    int i;
    //initialize all entries of the hash to NULL
    for(i =0; i <hash->capacity;++i)
    hash->array[i]=NULL;
    
    return hash;
}

//check if there is slot available
int isFull(Queue* queue)
{
    return queue-> count== queue->numberFrames;
}

//check if queue is empty
int isEmpty(Queue* queue)
{
    return queue->tail == NULL;
}

//delete frame from queue
void dequeue(Queue* queue)
{
    if(isEmpty(queue))
        return;
    
    //if there is only one node in the list, then change head to NULL
    if(queue->head == queue->tail)
        queue->head = NULL;
    
    //change tail and remove the previous tail    
    Node* temp = queue->tail;
    queue-> tail = queue->head->prev;
    
    if(queue)
        queue->tail->next = NULL;
    
    free(temp);
    
    //decrement the number of full frames by 1
    queue->count--;
}

void enqueue(Queue* queue, Hash* hash, int pageNumber)
{
    //if full, then remove the frame as the tail
    if(isFull(queue))
    {
        //remive page from hash
        hash->array[queue->tail->pageNumber]=NULL;
        dequeue(queue);
    }
    
    //create a new node with given pageNumber
    Node* temp = createNode(pageNumber);
    //add the new node to the front of queue
    temp->next = queue->head;
    
    //if queue is empty, change both head and tail pointers
    if(isEmpty(Queue))
    queue->tail= queue->front= temp;
    //else change only the head pointer
    else
    {
        queue->head->prev=temp;
        queue->head=temp;
    }
    
    //add page entry to hash
    hash->array[pageNumber]= temp;
    //increment number of full frames
    queue->count++;
}

void LRU(Queue* queue, Hash*hash, int pageNumber)
{
    //see if the requesting pageNumber is in the memory
    Node* reqPage = hash-> array[pageNumber];
    
    //if the page is not in the memory, bring it in to to memory
    if(reqPage == NULL)
        enqueue(queue, hash, pageNumber);
    
    //page is there and not at the head, change pointer
    else if(reqPage  != queue->head)
    {
        //unlink requested page from its current location in queue
        reqPage-> prev-> next= reqPage->next;
        if(reqPage->next)
            reqPage->prev= reqPage->prev;
        
        //if the requested page is at tail, then move it to the head
        if(reqPage == queue->tail)
        {
            queue->tail = reqPage->prev;
            queue->tail->next=NULL;
        }
        
        //put the requested page before current head
        reqPage->next= queue->front;
        reqPage->prev =NULL;
        
        //change prev of current head
        reqPage-> prev = reqPage;
        
        //change head to the requested page
        queue->head=reqPage;
    }
}
// CLOCK (Extra)

// LFU (Extra)

// LFU_K (Extra)

/************************************************************
 *     Buffer Manager Interface Pool Handling               *
 ************************************************************/

//Written 2016/02/18
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData) {

	// Create the Buffer Pool, which is an array of #numPages FrameInfo structs
	FrameInfo *bufferPool = (FrameInfo *) malloc(numPages * sizeof(FrameInfo));

	// Initialize the Frames inside the Buffer Pool
	// TODO: MAKE THIS A SEPARATE FUNCTION

	int i = 0;
	while (i < numPages) {
		// Allocate the memory for the Frame (same as a page)
		bufferPool[i].frameData = (char *) malloc(PAGE_SIZE * sizeof(char));
		// Set the frame number
		bufferPool[i].frameNumber = i;
		// Set the page number to -1, since we are initializing the buffer
		bufferPool[i].pageNumber = NO_PAGE;
		// Set fixCount to 0 and isDirty to false, since we are initializing the buffer
		bufferPool[i].fixCount = 0;
		bufferPool[i].isDirty = false;

		// Next, fill the information for the first and last buffers
		if (i == 0) {
			bufferPool[i].previousFrame = NULL;
		} else {
			bufferPool[i].previousFrame = &bufferPool[i - 1];
		}
		if (i == numPages - 1) {
			bufferPool[i].nextFrame = NULL;
		} else {
			bufferPool[i].nextFrame = &bufferPool[i + 1];
		}

		i++;
	}

	// Create the BufferPoolInfo variable
	BufferPoolInfo *buffPoolInfo = (BufferPoolInfo *) malloc(sizeof(BufferPoolInfo));

	// Fill the BufferPoolInfo:

	// Assign the pointer to the Buffer Pool array
	buffPoolInfo->bufferPool = bufferPool;

	// Assign the pointers to the first and last frames
	buffPoolInfo->firstFrame = &bufferPool[0];
	buffPoolInfo->lastFrame = &bufferPool[numPages - 1];

	// Set number of reads and writes to 0
	buffPoolInfo->readNumber = 0;
	buffPoolInfo->writeNumber = 0;

	// Fill the  BM_BufferPool struct
	bm->pageFile = (char *) pageFileName;
	bm->numPages = numPages;
	bm->strategy = strategy;
	bm->mgmtData = buffPoolInfo;

	return RC_OK;
}

//Written 2016/02/18
RC shutdownBufferPool(BM_BufferPool *const bm) {
	// Get number of pages to iterate
	int numPages = bm->numPages;

	// Create the BufferPoolInfo pointer and point it to the mgmt information
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;

	// Get the information about pinned pages and dirty pages
	int *fixCounts = getFixCounts(bm);
	bool *dirtyFlags = getDirtyFlags(bm);

	// Check if there are any frames accessed by any client(s)
	for (int i = 0; i < numPages; i++) {
		// If there is any, abort shutdown
		if (*(fixCounts + i) != 0) {
			// TODO: Maybe create an extra error code for this?
			return RC_WRITE_FAILED
		}
	}

	// Check if there are any frames with unsaved changes
	for (int i = 0; i < numPages; i++) {
		// If there are dirty frames, save changes by calling forceFlushPool
		if (*(dirtyFlags + i) == true) {
			forceFlushPool(bm);
			// Not need to keep checking
			break;
		}
	}

	free(buffPoolInfo);

	return RC_OK;
}

//Written 2016/02/18
//Edited 2016/02/19
RC forceFlushPool(BM_BufferPool *const bm) {
	// Get number of pages to iterate
	int numPages = bm->numPages;

	char *fileName = bm->pageFile;
	SM_FileHandle fileHandle;
	SM_PageHandle pageHandle = (SM_PageHandle) malloc(sizeof(SM_PageHandle));

	// Create the BufferPoolInfo pointer and point it to the mgmt information
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;

	// Check for dirty flags
	bool *dirtyFlags = getDirtyFlags(bm);

	int *fixCounts = getFixCounts(bm);

	for (int = 0; i < numPages; i++) {
		if ((*(dirtyFlags + i) == true) && (*(fixCounts + i) == 0)) {
			// Load frame
			FrameInfo *frame = &(buffPoolInfo->bufferPool[i]);

			PageNumber pageNum = frame->pageNumber;

			// Copy frame to pageHandle
			strcpy(pageHandle, frame->frameData);
			// Open file
			openPageFile(fileName, &fileHandle);
			// Write page to disk
			writeBlock(pageNum, &fileHandle, pageHandle);
			// DO I NEED TO CLOSE IT?
			closePageFile(&fileHandle);
			// Set dirty flag as false
			frame->isDirty = false;
			// Increase number of pages written
			buffPoolInfo->writeNumber++;

		}
	}
}

/************************************************************
 *     Buffer Manager Interface Access Pages                *
 ************************************************************/

//Written 2016/02/26 Pat
//Edited  2016/02/28
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page) {
	
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;
	
	// Get number of pages on buffer pool
	int numPages = bm->numPages;
	
	if(numPages >= page->pageNum)
			// Mark as dirty
			bufferPool[page->pageNum]->isDirty = true;
			return RC_OK;
	}
	// Page not found
	return RC_WRITE_FAILED;
}

//Written 2016/02/27 Pat
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;
	
	// Get number of pages on buffer pool
	int numPages = bm->numPages;
	if(numPages <= page->pageNum)
	{
		bufferPool[page->pageNum]->fixCount--;
		return RC_OK;
	}
	// Page not found
	return RC_WRITE_FAILED;
}

//Written 2016/02/27 Pat
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	
	//If buffer pool exists
	if(buffPoolInfo!=NULL)
	{
		//Write back to disk
		writeBlock(page->pageNum,buffPoolInfo,page->data);
		//Increase write time
		buffPoolInfo->writeNumber++;
		return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

//Written 2016/02/27
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
            const PageNumber pageNum) {
	

	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;
	
	// Get number of pages on buffer pool
	int numPages = bm->numPages;
	
	if(numPages = page->pageNum)
	{
		bufferPool[page->pageNum]->fixCount++;
		
	}
	else
	{
		//read page using replacement strategy
		//TODO: add other strategy
		doFifo(bm,buffPoolInfo->firstFrame->pageNumber,page->pageNum);
		bufferPool[page->pageNum]->fixCount++;
	}
	
	//data field points to page frame
	//data->numPages;

	return RC_OK;
}

}

/************************************************************
 *                   Statistics Interface                   *
 ************************************************************/

//Written 2016/02/19
PageNumber *getFrameContents (BM_BufferPool *const bm) {

	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;

	// Get number of pages to iterate
	int numPages = bm->numPages;

	PageNumber *frameContents = (PageNumber *) malloc(numPages * sizeof(PageNumber));

	for (int i = 0; i < numPages; i++){
		frameContents[i] = bufferPool[i].pageNumber;
	}

	return frameContents;
}

//Written 2016/02/18
bool *getDirtyFlags (BM_BufferPool *const bm) {
	// TODO: Iterate through bufferPool array and copy the values (TRUE/FALSE)

	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;

	// Get number of pages to iterate
	int numPages = bm->numPages;

	// Create array of bools with numPages size
	bool *dirtyFlags = (bool *) malloc(numPages * sizeof(bool));

	// Iterate through the frames and fill this array
	for (int i = 0; i < numPages; i++) {
		dirtyFlags[i] = bufferPool[i].isDirty;
	}

	return dirtyFlags;

}

//Written 2016/02/18
int *getFixCounts (BM_BufferPool *const bm) {
	// TODO: Iterate through bufferPool array and copy the values (int)

	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;

	// Get number of pages to iterate
	int numPages = bm->numPages;

	int *fixCounts = (int *) malloc(numPages * sizeof(int));

	for (int i = 0; i < numPages; i++) {
		fixCounts[i] = bufferPool[i].fixCount;
	}

	return fixCounts;

}

//Written 2016/02/19
int getNumReadIO (BM_BufferPool *const bm) {
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	return buffPoolInfo->readNumber;
}

//Written 2016/02/19
int getNumWriteIO (BM_BufferPool *const bm) {
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	return buffPoolInfo->writeNumber;
}
