#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <sys/stat.h>


RC checkDoesFileHandleExist(SM_FileHandle *fHandle)
{
	if (fHandle == NULL)
		return RC_SM_NOT_FOUND;
	else
		return RC_OK;
}

void initStorageManager (void) {

}

/************************************************************
 *              manipulating page files                     *
 ************************************************************/


/*******************************************************************
* NAME :            RC createPageFile (char *fileName)
*
* DESCRIPTION :     Create a new page file fileName. The initial size is one page.
*					This method fills this single page with '\0' bytes.
*
* PARAMETERS:
*            char *  fileName                Name of the file to be created
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  file created successfully
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-01		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC createPageFile (char *fileName) {
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

/*******************************************************************
* NAME :            RC openPageFile (char *fileName, SM_FileHandle *fHandle)
*
* DESCRIPTION :     Opens an existing page file. If opening the file is successful,
*                   then the fields of this file handle should be initialized with
*                   the information about the opened file.
*
* PARAMETERS:
*            char *  fileName                Name of the file to be created
*            SM_Filehandle * fHandle         An existing file handle
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  file created successfully
*					 RC_FILE_NOT_FOUND      file does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-01		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC openPageFile (char *fileName, SM_FileHandle *fHandle) {
	// Open an existing file for both reading and writing
	FILE *pageFile = fopen(fileName, "rb+");

	// Check if file exists
	if (pageFile == NULL) {
		return RC_FILE_NOT_FOUND;
	} else {
		// Calculate size of the file
		struct stat st;
		stat(fileName, &st);
		int size = st.st_size;

		// Obtain number of pages
		int nPages = (int) size / PAGE_SIZE;

		// Initialize fields of file handle
		fHandle->fileName = fileName;
		fHandle->totalNumPages = nPages;
		fHandle->curPagePos = 0;
		fHandle->mgmtInfo = pageFile;
	}
	return RC_OK;
}

/*******************************************************************
* NAME :            closePageFile (SM_FileHandle *fHandle)
*
* DESCRIPTION :     Close page file which already opened stored information in fHandle.
*                   If closing the file is successful, then return RC_OK,
*                   else return error.
*
* PARAMETERS
*            SM_Filehandle * fHandle         An existing file handle
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  file created successfully
*					 RC_FILE_NOT_FOUND      file does not exist
*
* AUTHOR :
*			 Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	  WHO     				                                      DETAIL
*            -----------    -----------------------------------------------------      ---------------------------------
*            2015-02-01	 Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>      Initialization
*            2015-02-06     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>      Added comments and header comment
*
*******************************************************************/
RC closePageFile (SM_FileHandle *fHandle) {

	//Check and close file return error if any
	int close = fclose(fHandle->mgmtInfo);
	if (close == 0)
	{
		return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

/*******************************************************************
* NAME :            RC destroyPageFile (char *fileName)
*
* DESCRIPTION :     Destroys an existing page file.
*
* PARAMETERS:
*            char *  fileName                Name of the file to be created
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  file destroyed successfully
*					 RC_FILE_NOT_FOUND      file does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-01		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC destroyPageFile (char *fileName) {
	// Check if file is removed successfully
	int removed = remove(fileName);
	if (removed == 0) {
		return RC_OK;
	} else {
		return RC_FILE_NOT_FOUND;
	}
}

/*******************************************************************
* NAME :            readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Seek into specific page and perform read page in to memory if it valid
*
* PARAMETERS
*            int pageNum                     Number of page needed to be read
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Memory which wanted to load data into
*
* RETURN :
*            Type:   RC                     Returned code:
*            Values: RC_OK                  file created successfully
*					RC_READ_NON_EXISTING_PAGE      page file does not exist
*
* AUTHOR :
*			 Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	  WHO     				                                      DETAIL
*            -----------    -----------------------------------------------------      ---------------------------------
*            2015-02-01	 Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>      Initialization
*            2015-02-06     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>      Add missing fseek
*            2015-02-06     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>      Added comments and header comment
*
*******************************************************************/
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	int pageFirstByte = pageNum * PAGE_SIZE * sizeof(char);

	//Check if page does exist
	if (fHandle->totalNumPages < pageNum ||  pageNum < 0)
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
	else
	{
		//Set current page position
		fHandle->curPagePos = pageNum;
		//allocate memory to memPage
		memPage = (char*)malloc(sizeof(char) * PAGE_SIZE);
		//Read and write block to memPage (Expected number of element read should be return)
		fseek(fHandle->fileName,pageFirstByte,SEEK_END);
		if (fread(memPage, 1, PAGE_SIZE, fHandle->mgmtInfo) == PAGE_SIZE)
		{

			return RC_OK;
		}
		else
		{
			return RC_READ_NON_EXISTING_PAGE;
		}
	}
}

/*******************************************************************
* NAME :            int getBlockPos (SM_FileHandle *fHandle)
*
* DESCRIPTION :     Return the current page position in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*
* RETURN :
*            Type:   int                    Current page position
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
int getBlockPos (SM_FileHandle *fHandle) {
	// Get the current page from the file handle information
	return fHandle->curPagePos;
}

/*******************************************************************
* NAME :            RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Read the first page in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Pointer to an area in memory storing the data of a page
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK                          file read successfully
*					 RC_READ_NON_EXISTING_PAGE      page does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// Call readBlock to read page '0' and return its RC
	return readBlock(0, fHandle, memPage);
}

/*******************************************************************
* NAME :            RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Read the previous page to the current position in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Pointer to an area in memory storing the data of a page
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK                          file read successfully
*					 RC_READ_NON_EXISTING_PAGE      page does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// Calculate previous page
	int previousBlock = fHandle->curPagePos - 1;

	// Call readBlock to read previousBlock and return its RC
	return readBlock(previousBlock, fHandle, memPage);
}

/*******************************************************************
* NAME :            RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Read the current page in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Pointer to an area in memory storing the data of a page
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK                          file read successfully
*					 RC_READ_NON_EXISTING_PAGE      page does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// Call readBlock to read current page and return its RC
	return readBlock(fHandle->curPagePos, fHandle, memPage);
}

/*******************************************************************
* NAME :            RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Read the next page to the current position page in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Pointer to an area in memory storing the data of a page
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK                          file read successfully
*					 RC_READ_NON_EXISTING_PAGE      page does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// Calculate next page
	int nextBlock = fHandle->curPagePos + 1;

	// Call readBlock to read nextBlock and return its RC
	return readBlock(nextBlock, fHandle, memPage);
}

/*******************************************************************
* NAME :            RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Read the last page in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Pointer to an area in memory storing the data of a page
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK                          file read successfully
*					 RC_READ_NON_EXISTING_PAGE      page does not exist
*
* AUTHOR :
*			 Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2015-02-06     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// Call readBlock to read last page (totalNumPages) and return its RC
	return readBlock(fHandle->totalNumPages, fHandle, memPage);
}

/************************************************************
 *            writing blocks to a page file                 *
 ************************************************************/

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {

}
/*******************************************************************
* NAME :            RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Write the current page in a file.
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*            SM_PageHandle memPage           Pointer to an area in memory storing the data of a page
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK                          file write successfully
*		     RC_WRITE_FAILED                failed to write
*
* AUTHOR :
*			Yung Chi Shih <yshih2@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Yung Chi Shih <yshih2@hawk.iit.edu>   Initialization
*            2015-02-07     	Yung Chi Shih <yshih2@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	//check if the file handle exists
	checkDoesFileHandleExist(fHandle);
	
	// Call writeBlock to write current page and return its RC
	return writeBlock(fHandle->curPagePos, fHandle, memPage);

}
/*******************************************************************
* NAME :            RC appendEmptyBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
*
* DESCRIPTION :     Append a new page at the end of the file
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_WRITE_FAILED                failed to write  
*					 
*
* AUTHOR :
*			 Yung Chi Shih <yshih2@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-02		Yung Chi Shih <yshih2@hawk.iit.edu>   Initialization
*            2015-02-07    	Yung Chi Shih <yshih2@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/

RC appendEmptyBlock (SM_FileHandle *fHandle) {
	//check if the file handle exists
	checkDoesFileHandleExist(fHandle);

	//sets the file position of the stream to the end of file
	fseek(pageFile, 0, SEEK_End);
	
	//gets the current file position
	int len = ftell(pageFile);
	
	//initializes an array with a size of PAGE_SIZE
	char buffer[PAGE_SIZE] = "";
	//writes data 
	fwrite(buffer, sizeof(buffer), 1, pageFile);
	
	//check if a new block of size PAGE_SIZE is appeneded
	if (len + sizeof(buffer) != ftell(pageFile))
		return RC_WRITE_FAILED;
		
	//increments the total number of pages
	fHandle -> totalNumPages ++;
}

/*******************************************************************
* NAME :            RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
*
* DESCRIPTION :     Increase page size of file to equal numberOfPages if it is less than numberOfPages. 
*
* PARAMETERS:
*            SM_Filehandle * fHandle         An existing file handle
*
* RETURN :
*            Type:   RC                             Returned code:
*            Values: RC_OK			                file page size is equal to or more than numberOfPages  
*					 
*
* AUTHOR :
*			 Arpita Rathore <arathore@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2015-02-04		Arpita Rathore <arathore@hawk.iit.edu>   Initialization
*            2015-02-07    	Arpita Rathore <arathore@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
	//check if file handle exists
	checkDoesFileHandleExist(fHandle);

	//check if the file page size is less than numberOfPages
	if (fHandle->totalNumPages < numberOfPages)
		//call appendEmptyBlock to append an empty page, check again if size is still less than numberOfPages, continue appending a page until size is equal to numberOfPages 	
		do {
			appendEmptyBlock(fHandle);
		} while (fHandle->totalNumPages < numberOfPages);
	//file page size is equal to or more than numberOfPages, appending is not needed	
	else
		return RC_OK;

}
