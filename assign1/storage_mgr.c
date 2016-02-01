#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"

/* manipulating page files */
extern void initStorageManager (void) {

}

extern RC createPageFile (char *fileName) {
	// Open a new file for writing
	FILE *pageFile = fopen(fileName, "wb");

	// Allocate one PAGE_SIZE space in memory and fill with /0 characters
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
		// TODO: Some logic to get the size of the file
		// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
		// https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
		// https://www.securecoding.cert.org/confluence/display/c/FIO19-C.+Do+not+use+fseek()+and+ftell()+to+compute+the+size+of+a+regular+file


		fHandle->fileName = fileName;
		// fHandle->totalNumPages = 
		fHandle->curPagePos = 0;
		fHandle->mgmtInfo = pageFile;

	}

	return RC_OK;
}

extern RC closePageFile (SM_FileHandle *fHandle) {

}

extern RC destroyPageFile (char *fileName) {

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
