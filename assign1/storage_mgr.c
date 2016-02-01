#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <sys/stat.h>


RC checkDoesFileHandleExist(SM_FileHandle *fHandle)
{
	if(fHandle == NULL)
		return RC_SM_NOT_FOUND;
	else
		return RC_OK;
}
/* manipulating page files */
extern void initStorageManager (void) {

}

extern RC createPageFile (char *fileName) {
	// Open a new file for writing
	FILE *pageFile = fopen(fileName, "wb");

	// Allocate one PAGE_SIZE space in memory and fill with \0 characters
	char *buffer = (char *) calloc(PAGE_SIZE, sizeof(char));

	// Write the buffer of zeroes to into the file
	fwrite (buffer, sizeof(char), PAGE_SIZE, pageFile);

	// Free memory and close
	free(buffer);
	fclose(pageFile);

	return RC_OK;
}

extern RC openPageFile (char *fileName, SM_FileHandle *fHandle) {
	// Open an existing file for both reading and writing
	FILE *pageFile = fopen(fileName, "rb+");

	if (pageFile == NULL) {
		return RC_FILE_NOT_FOUND;
	} else {
		
		struct stat st;
		stat(fileName, &st);
		int size = st.st_size;

		int nPages = (int) size/PAGE_SIZE;

		fHandle->fileName = fileName;
		fHandle->totalNumPages = nPages;
		fHandle->curPagePos = 0;
		fHandle->mgmtInfo = pageFile;
	}
	return RC_OK;
}

extern RC closePageFile (SM_FileHandle *fHandle) {
	//Check and close file return error if any
	int close = fclose(fHandle->mgmtInfo);
	if(close == 0)
	{
		return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

extern RC destroyPageFile (char *fileName) {
	int removed = remove(fileName);
	if (removed == 0) {
		return RC_OK;
	} else {
		return RC_FILE_NOT_FOUND;
	}
}

/* reading blocks from disc */
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern int getBlockPos (SM_FileHandle *fHandle) {

}

extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

/* writing blocks to a page file */
extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {

}

extern RC appendEmptyBlock (SM_FileHandle *fHandle) {

}

extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {

}
