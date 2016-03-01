#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Struct that contains the essential information of all the frames in the buffer pool.
The buffer pool itself is an array of elements of this struct.
*/
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

/*
Struct that contains the buffer pool itself, as well as essential information needed for
management purposes.
*/
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
	//number of filled frames
	int count;
} BufferPoolInfo;

/************************************************************
 *                 Replacement Strategies                   *
 ************************************************************/

// FIFO
//Written 2016-02-27 Pat
//Edited 2016-02-29
//Edited 2016-03-01
RC doFifo(BM_BufferPool *const bm, BM_PageHandle *const page,PageNumber pageNum)
{
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	if(buffPoolInfo->firstFrame->nextFrame!=NULL)
	{
		//If dirty write it down to disk
		if(buffPoolInfo->firstFrame->isDirty)
		{
			SM_FileHandle fileHandle;
			char *fileName = bm->pageFile;
			
			// Open file
			openPageFile(fileName, &fileHandle);
			// Write page to disk
			writeBlock(buffPoolInfo->firstFrame->pageNumber, &fileHandle, buffPoolInfo->firstFrame->frameData);
			// Close page file
			closePageFile(&fileHandle);
			buffPoolInfo->writeNumber++;
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
		buffPoolInfo->lastFrame->nextFrame->previousFrame = &buffPoolInfo->lastFrame;
		buffPoolInfo->lastFrame = &buffPoolInfo->lastFrame->nextFrame;
		buffPoolInfo->lastFrame->pageNumber = pageNum;
		page->data = buffPoolInfo->lastFrame->frameData;
		return RC_OK;
	}
	return RC_READ_NON_EXISTING_PAGE;
}
// LRU
typedef struct Hash
{
    //number of frames that can be stored
    int capacity
    //an array of nodes
    FrameInfo* *array
}

FrameInfo* createNode(const int frameNumber)
{
    //allocate memory 
    FrameInfo* temp = (FrameInfo *)malloc( sizeof(FrameInfo));
    //assign the given frameNumner to the temp Node
    temp -> frameNumber = frameNumber;
    //initialize prev and next pointer as NULL
    temp->previousFrame= temp->nextFrame=NULL;
    return temp;
}

BufferPoolInfo* createQueue( int numPages)
{
    //allocate memory
    BufferPoolInfo* bufferPool = (BufferPoolInfo *)malloc(sizeof(BufferPoolInfo));
    //set the count to 0
    bufferPool->count = 0;
    //assign the head and tail as NULL
    bufferPool-> firstFrame = bufferPool->lastFrame= NULL;
    
    //number of frames that can be stored
    bufferPool->numPages = numPages;
}

Hash* createHash(PageNumber pageNumber)
{
    //allocate memory
    Hash* hash =(Hash *)malloc(sizeof(Hash));
    //assign the capacity
    hash-> capacity = pageNumber;
    //create an array of index
    hash-> array =(FrameInfo**)malloc(hash->capacity * sizeof(Node));
    int i;
    //initialize all entries of the hash to NULL
    for(i =0; i <hash->capacity;++i)
    hash->array[i]=NULL;
    
    return hash;
}

//check if there is slot available
int isFull(BufferPoolInfo* bufferPool)
{
    return bufferPool-> count == bufferPool->frameNumber;
}

//check if queue is empty
int isEmpty(BufferPoolInfo* bufferPool)
{
    return bufferPool->lastFrame == NULL;
}

//delete frame from queue
void dequeue(BufferPoolInfo* bufferPool)
{
    if(isEmpty(bufferPool))
        return;
    
    //if there is only one node in the list, then change head to NULL
    if(bufferPool->firstFrame == bufferPool->lastFrame)
        bufferPool->firstFrame = NULL;
    
    //change tail and remove the previous tail    
    FrameInfo* temp = bufferPool->lastFrame;
    bufferPool-> lastFrame = bufferPool->firstFrame->previousFrame;
    
    if(bufferPool->lastFrame)
        bufferPool->lastFrame->nextFrame = NULL;
    
    free(temp);
    
    //decrement the number of full frames by 1
    bufferPool->count--;
}

void enqueue(BufferPoolInfo* bufferPool, Hash* hash, PageNumber pageNumber)
{
    //if full, then remove the frame as the lastFrame
    if(isFull(bufferPool))
    {
        //remive page from hash
        hash->array[bufferPool->lastFrame->pageNumber]=NULL;
        dequeue(bufferPool);
    }
    
    //create a new node with given pageNumber
    FrameInfo* temp = createNode(pageNumber);
    //add the new node to the front of queue
    temp->nextFrame = bufferPool->firstFrame;
    
    //if queue is empty, change both head and firstFrame pointers
    if(isEmpty(BufferPoolInfo))
    bufferPool->lastFrame= bufferPool->firstFrame= temp;
    //else change only the head pointer
    else
    {
        bufferPool->firstFrame->previousFrame=temp;
        bufferPool->firstFrame=temp;
    }
    
    //add page entry to hash
    hash->array[pageNumber]= temp;
    //increment number of full frames
    bufferPool->count++;
}

RC LRU(BM_BufferPool *const bm, BM_PageHandle *const page, Hash*hash, PageNumber pageNumber)
{
    
    BufferPoolInfo *buffPoolInfo = bm->mgmtData;
    //see if the requesting pageNumber is in the memory
    FrameInfo* reqPage = hash-> array[pageNumber];
    
    //if the page is not in the memory, bring it in to to memory
    if(reqPage == NULL)
    {
        RC readToMem = readBlock(pageNum, bm->mgmtData, buffPoolInfo->frameData);
		if(readToMem != RC_OK)
		{
			return RC_READ_NON_EXISTING_PAGE;
		}
        enqueue(buffPoolInfo, hash, pageNumber);
    }
    //page is there and not at the head, change pointer
    else if(reqPage  != buffPoolInfo->firstFrame)
    {
        //unlink requested page from its current location in queue
        reqPage-> previousFrame-> nextFrame= reqPage->nextFrame;
        if(reqPage->nextFrame)
            reqPage->previousFrame= reqPage->previousFrame;
        
        //if the requested page is at tail, then move it to the head
        if(reqPage == buffPoolInfo->lastFrame)
        {
            bufferPoolInfo->lastFrame = reqPage->previousFrame;
            bufferPoolInfo->lastFrame->nextFrame=NULL;
        }
        
        //put the requested page before current head
        reqPage->nextFrame= buffPoolInfo->firstFrame;
        reqPage->previousFrame =NULL;
        
        //change prev of current head
        reqPage-> previousFrame = reqPage;
        
        //change head to the requested page
        buffPoolFrame->firstFrame=reqPage;
    }
}
// CLOCK (Extra)

// LFU (Extra)

// LFU_K (Extra)

/************************************************************
 *     Buffer Manager Interface Pool Handling               *
 ************************************************************/

/*******************************************************************
* NAME :            RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
*                  						const int numPages, ReplacementStrategy strategy,
*                  						void *stratData)
*
* DESCRIPTION :     Creates a new buffer pool with numPages page frames using the page replacement
*					given in the parameters
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*			 const char *const pageFileName		Name of the file to read pages from
*			 const int numPages 				Size of the buffer pool
*			 ReplacementStrategy strategy 		Replacement strategy followed
*			 void *stratData 					Extra parameters
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  buffer pool created successfully
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-18	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData) {
	// Create the Buffer Pool, which is an array of #numPages FrameInfo structs
	FrameInfo *bufferPool = (FrameInfo *) malloc(numPages * sizeof(FrameInfo));

	// Initialize the Frames inside the Buffer Pool
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

		// Next, fill the information for the previous and next frames for each frame
		if (i == 0) {
			// First frame has no previous frame
			bufferPool[i].previousFrame = NULL;
		} else {
			bufferPool[i].previousFrame = &bufferPool[i - 1];
		}
		if (i == numPages - 1) {
			// Last frame has no next frame
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

/*******************************************************************
* NAME :            RC shutdownBufferPool(BM_BufferPool *const bm)
*
* DESCRIPTION :     Destroys a buffer pool. This method also frees up all resources 
*					associated with the buffer pool.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  buffer pool destroyed successfully
*					 RC_RC_PINNED_PAGES		error while trying to shutdown, caused by pinned pages
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-18	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
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
			// Return custom error because there are pinned files
			return RC_PINNED_PAGES;
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

	// Free up resources and return RC code
	free(buffPoolInfo);
	
	return RC_OK;
}

/*******************************************************************
* NAME :            RC forceFlushPool(BM_BufferPool *const bm)
*
* DESCRIPTION :     Writes all dirty pages (with fix count 0) from the buffer pool to disk.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  dirty pages written successfully
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-18	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*			 2016-02-19     Adrian Tirados <atirados@hawk.iit.edu>   Edited
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC forceFlushPool(BM_BufferPool *const bm) {
	// Get number of pages to iterate
	int numPages = bm->numPages;

	// Get the information about the page from the buffer pool and start a fileHandle
	char *fileName = bm->pageFile;
	SM_FileHandle fileHandle;
	SM_PageHandle pageHandle = (SM_PageHandle) malloc(sizeof(SM_PageHandle));

	// Create the BufferPoolInfo pointer and point it to the mgmt information
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;

	// Check for dirty flags
	bool *dirtyFlags = getDirtyFlags(bm);

	int *fixCounts = getFixCounts(bm);

	for (int i = 0; i < numPages; i++) {
		// If dirty flag and not pinned
		if ((*(dirtyFlags + i) == true) && (*(fixCounts + i) == 0)) {
			
			// Load frame and page number
			FrameInfo *frame = &(buffPoolInfo->bufferPool[i]);
			PageNumber pageNum = frame->pageNumber;

			// Copy frame to pageHandle
			strcpy(pageHandle, frame->frameData);
			// Open file
			openPageFile(fileName, &fileHandle);
			// Write page to disk
			writeBlock(pageNum, &fileHandle, pageHandle);
			// Close page file
			closePageFile(&fileHandle);
			// Set dirty flag as false
			frame->isDirty = false;
			// Increase number of pages written
			buffPoolInfo->writeNumber++;
		}
	}

	// free(dirtyFlags);
	// free(fixCounts);

	return RC_OK;
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
//Edited  2016/02/29
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;
	
	// Get number of pages on buffer pool
	int numPages = bm->numPages;
	if(numPages >= page->pageNum)
	{
		if(bufferPool[page->pageNum]->isDirty)
		{
			forcePage(bm,page);
		}
		bufferPool[page->pageNum]->fixCount--;
		return RC_OK;
	}
	// Page not found
	return RC_WRITE_FAILED;
}

//Written 2016/02/27 Pat
//Edited  2016/02/29
//Edited  2016/03/01
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	
	//If buffer pool exists
	if(buffPoolInfo!=NULL)
	{
		SM_FileHandle fileHandle;
		char *fileName = bm->pageFile;
		
		// Open file
		openPageFile(fileName, &fileHandle);
		// Write page to disk
		writeBlock(page->pageNum, &fileHandle, page->data);
		// Close page file
		closePageFile(&fileHandle);
		//Write back to disk
		//writeBlock(page->pageNum,buffPoolInfo,page->data);
		//Increase write time
		buffPoolInfo->writeNumber++;
		//Clear Dirty Flag
		buffPoolInfo->bufferPool[page->pageNum]->isDirty = false;
		return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

//Written 2016/02/27
//Edited 2016/02/29
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
            const PageNumber pageNum) {
	

	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;
	
	// Get number of pages on buffer pool
	int numPages = bm->numPages;
	
	//pins pageNum page
	if(numPages = page->pageNum)
	{
		bufferPool[page->pageNum]->fixCount++;
		return RC_OK;
	}
	/*else
	{
		//read page using replacement strategy
		//TODO: add other strategy
		doFifo(bm,buffPoolInfo->firstFrame->pageNumber,page->pageNum);
		bufferPool[page->pageNum]->fixCount++;
	}
	*/
	//data field points to page frame
	//data->numPages;

	return RC_WRITE_FAILED;
}

}

/************************************************************
 *                   Statistics Interface                   *
 ************************************************************/

/*******************************************************************
* NAME :            PageNumber *getFrameContents (BM_BufferPool *const bm)
*
* DESCRIPTION :     Returns an array of PageNumbers (of size numPages) where the ith element is 
*					the number of the page stored in the ith page frame. 
*					An empty page frame is represented using the constant NO_PAGE.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   PageNumber *            
*            Values: integer representing the number of the page stored in the ith frame, NO_PAGE if empty.
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-19	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
PageNumber *getFrameContents (BM_BufferPool *const bm) {
	// Retrieve the information about the buffer pool
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;

	// Get number of pages to iterate
	int numPages = bm->numPages;

	// Allocate a buffer to write the page numbers with numPages size
	PageNumber *frameContents = (PageNumber *) malloc(numPages * sizeof(PageNumber));

	for (int i = 0; i < numPages; i++){
		// Obtain the information directly from the buffer pool struct
		frameContents[i] = bufferPool[i].pageNumber;
	}

	return frameContents;
}

/*******************************************************************
* NAME :            bool *getDirtyFlags (BM_BufferPool *const bm)
*
* DESCRIPTION :     Returns an array of bools (of size numPages) where the ith element 
*					is TRUE if the page stored in the ith page frame is dirty. 
*					Empty page frames are considered as clean.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   bool *            
*            Values: boolean representing if page stored in the ith frame is dirty (true) or not (false)
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-18	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
bool *getDirtyFlags (BM_BufferPool *const bm) {
	// Retrieve the information about the buffer pool
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;

	// Get number of pages to iterate
	int numPages = bm->numPages;

	// Create array of bools with numPages size
	bool *dirtyFlags = (bool *) malloc(numPages * sizeof(bool));

	// Iterate through the frames and fill this array
	for (int i = 0; i < numPages; i++) {
		// Obtain the information directly from the buffer pool struct
		dirtyFlags[i] = bufferPool[i].isDirty;
	}

	return dirtyFlags;
}

/*******************************************************************
* NAME :            int *getFixCounts (BM_BufferPool *const bm)
*
* DESCRIPTION :     Returns an array of ints (of size numPages) where the ith element is 
*					the fix count of the page stored in the ith page frame. 
*					Return 0 for empty page frames.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   int *            
*            Values: integer representing the number of clients that pinned the file
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-18	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
int *getFixCounts (BM_BufferPool *const bm) {
	// Retrieve the information about the buffer pool
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	FrameInfo *bufferPool = buffPoolInfo->bufferPool;

	// Get number of pages to iterate
	int numPages = bm->numPages;

	// Allocate a buffer of ints of numPages size
	int *fixCounts = (int *) malloc(numPages * sizeof(int));

	for (int i = 0; i < numPages; i++) {
		// Obtain the information directly from the buffer pool struct
		fixCounts[i] = bufferPool[i].fixCount;
	}

	return fixCounts;
}

/*******************************************************************
* NAME :            int getNumReadIO (BM_BufferPool *const bm)
*
* DESCRIPTION :     Returns the number of pages that have been read from disk since 
*					a buffer pool has been initialized.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   int *            
*            Values: integer representing the number of pages read from disk
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-19	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
int getNumReadIO (BM_BufferPool *const bm) {
	// Obtain the information directly from the buffer pool struct
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	return buffPoolInfo->readNumber;
}

/*******************************************************************
* NAME :            int getNumWriteIO (BM_BufferPool *const bm)
*
* DESCRIPTION :     Returns the number of pages written to the page file since 
*					the buffer pool has been initialized.
*
* PARAMETERS:
*            BM_BufferPool *const bm        	Stores specific information about a buffer pool
*
* RETURN :
*            Type:   int *            
*            Values: integer representing the number of pages written to the page file
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-02-19	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-29     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
int getNumWriteIO (BM_BufferPool *const bm) {
	// Obtain the information directly from the buffer pool struct
	BufferPoolInfo *buffPoolInfo = bm->mgmtData;
	return buffPoolInfo->writeNumber;
}
