CS 525 Assignment 4 04/28/2016 

B+- Tree Index: Implements a disk-based B+- tree index.

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
=========================================================================================================================

File List
----------

README.txt
Makefile
btree_mgr.c
btree_mgr.h
buffer_mgr.c
buffer_mgr.h
buffer_mgr_stat.c
buffer_mgr_stat.h
dberror.c
dberror.h
dt.h
expr.c
expr.h
record_mgr.c
record_mgr.h
rm_serializer.c
storage_mgr.c
storage_mgr.h
tables.h
test_assign4_1.c
test_expr.c
test_helper.h



Milestone
----------
2016-04-13	Initialization of methods
2016-04-18	Compiled code
2016-04-18  	Fixed all bugs and test passed OK

	



Installation instructions
--------------------------
The team has provided a Makefile to make the installation easier. 

To install the program, simply run the command 'make' in the folder where all the files are stored. This command will create a series of compiled .o files, and a single file called 'testOutput'. This file contains the compiled test case.

To run the program, execute './testOutput' in the folder where it is stored. This will automatically run the desired tests.

In addition, there is another test file provided. To execute it, simply run ./test_expr_Output.

Finally, in order to clean the directory, execute 'make clean', which will delete all .o files, all .bin files and testOutput file as well.


Function descriptions
----------------------

NAME: int searchKey (BTreeHandle *tree, Value *key)
DESCRIPTION: Search for the index of given key in tree. 


-- Init and Shutdown Index Manager --
NAME: RC initIndexManager (void *mgmtData)
DESCRIPTION: Initializes the index manager by setting a pointer to the memory that contains the B+-Tree with a size of 50.

NAME: RC shutdownIndexManager ()
DESCRIPTION: Terminates the index manager.


-- Create, Destroy, Open, and Close a Btree Index --
NAME: RC createBtree (char *idxId, DataType keyType, int n)
DESCRIPTION: Creates a B Tree with the given arguments.

NAME: RC openBtree (BTreeHandle **tree, char *idxId)
DESCRIPTION: Opens a B Tree identified by its id.

NAME: RC closeBtree (BTreeHandle *tree)
DESCRIPTION: Closes a B Tree passed as a parameter.

NAME: RC deleteBtree (char *idxId)
DESCRIPTION: Deletes a B Tree identified by its id.


-- Access Information about a B-Tree --
NAME: RC getNumNodes (BTreeHandle *tree, int *result)
DESCRIPTION: Gets the total number of nodes in the B Tree.

NAME: RC getNumEntries (BTreeHandle *tree, int *result)
DESCRIPTION: Get the number of nodes.

NAME: RC getKeyType (BTreeHandle *tree, DataType *result)
DESCRIPTION: Get data type of tree.


-- Index Access --
NAME: RC findKey (BTreeHandle *tree, Value *key, RID *result)
DESCRIPTION: Look for a given key in tree.

NAME: RC insertKey (BTreeHandle *tree, Value *key, RID rid)
DESCRIPTION: Insert new node to tree.

NAME: RC deleteKey (BTreeHandle *tree, Value *key)
DESCRIPTION: Delete key from tree.

NAME: RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle)
DESCRIPTION: Sort key in tree.

NAME: RC nextEntry (BT_ScanHandle *handle, RID *result)
DESCRIPTION: Calculates the next entry in the given scan.

NAME: RC closeTreeScan (BT_ScanHandle *handle)
DESCRIPTION: Free allocated memory from handle.


-- Debug and Test Functions --
NAME: *printTree (BTreeHandle *tree)
DESCRIPTION: Debug method.

=========================================================================================================================

Additional error codes: of all additional error codes
------------------------------------------------------
RC_TABLE_NOT_FOUND 402	Error produced when we cannot find a table on disk.


Data structure: main data structure used
-----------------------------------------
The data structure we use is:

1) BTreeInfo: this structure holds information about each node on the B+- Tree.

It has the following elements:

typedef struct BTreeInfo
{
	// Value structure that contains the data type of the record indexed by the node
	struct Value val;
	// RID structure that contains the page and slot identifiers for the indexed record
	struct RID rid;
} BTreeInfo;



Extra credit: of all extra credits
-----------------------------------

Additional files: of all additional files
------------------------------------------

Test cases: of all additional test cases added
-----------------------------------------------



Problems solved
----------------


Problems to be solved
----------------------
