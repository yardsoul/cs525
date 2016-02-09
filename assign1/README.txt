CS 525 Assignment 1 02/09/2016 

Storage Manager: Implements a storage manager that can read/write blocks to/from a file on disk.

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
dberror.c
dberror.h
Makefile
storage_mgr.c
storage_mgr.h
test_assign1_1.c
test_helper.h


Milestone
----------
2016-02-01	Initialization of methods
2016-02-06	Comments added
2016-02-08	Read and write block methods completed
2016-02-08	Compiled code
2016-02-08	Tested code in local
2016-02-08	Tested code remotely in fourier server


Installation instructions
--------------------------
The team has provided a Makefile to make the installation easier. 

To install the program, simply run the command 'make' in the folder where all the files are stored. This command will create a series of compiled .o files, and a single file called 'testOutput'. This file contains the compiled test case.

To run the program, execute './testOutput' in the folder where it is stored. This will automatically run the desired tests.

Finally, in order to clean the directory, execute 'make clean', which will delete all .o files, all .bin files and testOutput file as well.


Function descriptions
----------------------
NAME: RC createPageFile (char *fileName)
DESCRIPTION: Create a new page file fileName. The initial size is one page. This method fills this single page with '\0' bytes.

NAME: RC openPageFile (char *fileName, SM_FileHandle *fHandle)
DESCRIPTION: Opens an existing page file. If opening the file is successful, then the fields of this file handle should be initialized with the information about the opened file.

NAME: closePageFile (SM_FileHandle *fHandle)
DESCRIPTION: Close page file which already opened stored information in fHandle. If closing the file is successful, then return RC_OK, else return error.

NAME: RC destroyPageFile (char *fileName)
DESCRIPTION: Destroys an existing page file.

NAME: readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Seek into specific page and perform read page into memory if it is valid.

NAME: int getBlockPos (SM_FileHandle *fHandle)
DESCRIPTION: Return the current page position in a file.

NAME: RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Read the first page in a file.

NAME: RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Read the previous page to the current position in a file.

NAME: RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Read the current page in a file.

NAME: RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Read the next page to the current position page in a file.

NAME: RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Read the last page in a file.

NAME: RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Write a page to disk using an absolute position.

NAME: RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Write the current page in a file.

NAME: RC appendEmptyBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
DESCRIPTION: Append a new page at the end of the file.

NAME: RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
DESCRIPTION: Increase page size of file to equal numberOfPages if it is less than numberOfPages.
============================================================================================================================

Additional error codes: of all additional error codes
------------------------------------------------------
RC_WRITE_FAILED					failed to write
RC_READ_NON_EXISTING_PAGE		page does not exist
RC_FILE_NOT_FOUND				file does not exist



Data structure: main data structure used
-----------------------------------------
The two data structures used are File Handle SM_FileHandle and Page Handle SM_PageHandle. 
SM_FileHandle is a representation of an open page file which stores the file name, total number of pages in the file, and current page position. 
SM_PageHandle is a pointer to where the data of a page is stored in memory. 


Extra credit: of all extra credits
-----------------------------------

Additional files: of all additional files
------------------------------------------

Test cases: of all additional test cases added
-----------------------------------------------



Problems solved
----------------
This program implements a storage manager that can read blocks from a file on disk into memory and can write blocks from memory to a file on disk. It also provides methods for creating, opening, and closing files.


Problems to be solved
----------------------
