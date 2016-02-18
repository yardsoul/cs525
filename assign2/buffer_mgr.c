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
	FrameInfo * bufferPool;
	// First frame in the Buffer Pool
	FrameInfo *firstFrame;
	// Last frame in the Buffer Pool
	FrameInfo *lastFrame;

} BufferPoolInfo;

/************************************************************
 *     Buffer Manager Interface Pool Handling               *
 ************************************************************/

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData) {

	// Create the Buffer Pool, which is an array of #numPages FrameInfo structs
	FrameInfo *bufferPool = (FrameInfo *) malloc(numPages * sizeof(FrameInfo));

	// Initialize the Frames inside the Buffer Pool
	// TODO: MAKE THIS A SEPARATE FUNCTION
	// Use . notation or -> notation???
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

	// Fill the  BM_BufferPool struct
	bm->pageFile = (char *) pageFileName;
	bm->numPages = numPages;
	bm->strategy = strategy;
	bm->mgmtData = buffPoolInfo;

	return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const bm) {

}

RC forceFlushPool(BM_BufferPool *const bm) {

}

/************************************************************
 *     Buffer Manager Interface Access Pages                *
 ************************************************************/

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page) {

}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page) {

}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {

}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
            const PageNumber pageNum) {

}

/************************************************************
 *                   Statistics Interface                   *
 ************************************************************/

PageNumber *getFrameContents (BM_BufferPool *const bm) {

}

bool *getDirtyFlags (BM_BufferPool *const bm) {

}

int *getFixCounts (BM_BufferPool *const bm) {

}

int getNumReadIO (BM_BufferPool *const bm) {

}

int getNumWriteIO (BM_BufferPool *const bm) {

}