CS 525 Assignment 2 03/03/2016 

Buffer Manager: Implements a buffer manager that contains a buffer of blocks in memory through methods for reading and flushing to disk and block replacement.

Personnel Information
----------------------
Team members:
Patipat Duangchalomnin
Email: pduangchalomnin@hawk.iit.edu
Github username: pduangchalomnin
CWID: A20351140

Adrian Tirados Mata
Email: atirados@hawk.iit.edu
Github username: atirados
CWID: A20362186

Yung Chi Shih
Email: yshih2@hawk.iit.edu
Github username: yshih2
CWID: A20337574

Arpita Rathore (Team Representative)
Email: arathore@hawk.iit.edu
Github username: arathore1
CWID: A20340002
==========================================================================================================================

File List
----------

Makefile
buffer_mgr.h
buffer_mgr_stat.c
buffer_mgr_stat.h
dberror.c
dberror.h
dt.h
storage_mgr.c
storage_mgr.h
test_assign2_1.c
test_helper.h


Milestone
----------
2016-02-18	Initialization of methods
2016-02-29	Compiled code
2016-03-03  Fixed all bugs and test passed OK
	



Installation instructions
--------------------------
The team has provided a Makefile to make the installation easier. 

To install the program, simply run the command 'make' in the folder where all the files are stored. This command will create a series of compiled .o files, and a single file called 'testOutput'. This file contains the compiled test case.

To run the program, execute './testOutput' in the folder where it is stored. This will automatically run the desired tests.

Finally, in order to clean the directory, execute 'make clean', which will delete all .o files, all .bin files and testOutput file as well.


Function descriptions
----------------------
-- Strategy Functions --
NAME: RC manageBuffer(BM_BufferPool *const bm, BM_PageHandle *const page,PageNumber pageNum)
DESCRIPTION: Manage the position where the pages are loaded in the buffer based on the timestamp


-- Buffer Pool Functions --
NAME: RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
DESCRIPTION: Creates a new buffer pool with numPages page frames using the page replacement given in the parameters.

NAME: RC shutdownBufferPool(BM_BufferPool *const bm)
DESCRIPTION: Destroys a buffer pool. This method also frees up all resources associated with the buffer pool.

NAME: RC forceFlushPool(BM_BufferPool *const bm)
DESCRIPTION: Writes all dirty pages (with fix count 0) from the buffer pool to disk.


-- Page Management Functions --
NAME: RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
DESCRIPTION: Pins page in memory.

NAME: RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
DESCRIPTION: Unpins page in memory.

NAME: RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
DESCRIPTION: Marks page as dirty.

NAME: forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
DESCRIPTION: Force page from memory.


-- Statistics Functions --
NAME: PageNumber *getFrameContents (BM_BufferPool *const bm)
DESCRIPTION: Returns an array of PageNumbers (of size numPages) where the ith element is the number of the page stored in the ith page frame. An empty page frame is represented using the constant NO_PAGE.

NAME: bool *getDirtyFlags (BM_BufferPool *const bm)
DESCRIPTION: Returns an array of bools (of size numPages) where the ith element is TRUE if the page stored in the ith page frame is dirty. Empty page frames are considered as clean.

NAME: int *getFixCounts (BM_BufferPool *const bm)
DESCRIPTION: Returns an array of ints (of size numPages) where the ith element is the fix count of the page stored in the ith page frame. Return 0 for empty page frames.

NAME: int getNumReadIO (BM_BufferPool *const bm)
DESCRIPTION: Returns the number of pages that have been read from disk since a buffer pool has been initialized.

NAME: int getNumWriteIO (BM_BufferPool *const bm)
DESCRIPTION: Returns the number of pages written to the page file since the buffer pool has been initialized.

============================================================================================================================

Additional error codes: of all additional error codes
------------------------------------------------------
RC_PINNED_PAGES 400 -> Error while trying to shutdown, caused by pinned pages
RC_PAGE_NOT_FOUND 401 -> Cannot find page

Data structure: main data structure used
-----------------------------------------
We use two main data structures:

1) FrameInfo: this struct contains the essential information of all the frames in the buffer pool.
The buffer pool itself is an array of elements of this struct.

It has the following elements:

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
	//Time stamp
	long timeStamp;
} FrameInfo;


2) BufferPoolInfo: this struct contains the buffer pool itself, as well as essential information needed
management purposes.

It has the following elements:

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

Extra credit: of all extra credits
-----------------------------------

Additional files: of all additional files
------------------------------------------

Test cases: of all additional test cases added
-----------------------------------------------



Problems solved
----------------
- Fixed past errors from assignment 1 that were not covered in the test cases.

Problems to be solved
----------------------
