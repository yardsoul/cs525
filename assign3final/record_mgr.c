#include "record_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "expr.h"
#include <stdlib.h>
#include <string.h>

// Global variable that stores the name of the page file that contains the data
char pageFile[100];

// Struct that contains useful information for the scan functions
typedef struct ScanInfo
{
	// Current page scanned
	int currentPage;
	// Current slot scanned
	int currentSlot;
	// Total number of pages in the data file
	int totalNumPages;
	// Total number of slots per page
	int totalNumSlots;
	// Condition searched in the scan
	Expr *condition;
} ScanInfo;


/************************************************************
 *                     TABLE AND MANAGER                    *
 ************************************************************/

/*******************************************************************
* NAME :            RC initRecordManager (void *mgmtData)
*
* DESCRIPTION :     Initializes the record manager
*
* PARAMETERS:
*            	void *mgmtData					Pointer to memory reserved for management data
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record manager initialized succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC initRecordManager (void *mgmtData) {
	return RC_OK;
}

/*******************************************************************
* NAME :            RC shutdownRecordManager ()
*
* DESCRIPTION :     Terminates the record manager
*
* PARAMETERS:
*            	NONE
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record manager terminated successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC shutdownRecordManager () {
	return RC_OK;
}

/*******************************************************************
* NAME :            RC createTable (char *name, Schema *schema)
*
* DESCRIPTION :     Creates a table with the given name and schema, and stores the schema information
*					on the first page.
*
* PARAMETERS:
*            	char *name 						Name of the table to be created
*				Schema *schema 					Schema that will be used in the table
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Table created successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC createTable (char *name, Schema *schema) {
	// Declaring variables
	SM_FileHandle fileHandle;

	// Copy the name of the file in the global variable for posterior use
	strcpy(pageFile, name);

	// Create and open the page file
	createPageFile(name);
	openPageFile(name, &fileHandle);

	// Ensure capacity for the information on the first page
	ensureCapacity(1, &fileHandle);

	// Serialize the schema and write it on the first page
	char *schemaToString = serializeSchema(schema);
	writeBlock(0, &fileHandle, schemaToString);

	// Close the file and return the Return Code
	closePageFile(&fileHandle);
	return RC_OK;
}


/*******************************************************************
* NAME :            Schema *deserializeSchema (char *serializedSchema)
*
* DESCRIPTION :     Deserialize schema into variable
*
* PARAMETERS:
*            	char *serializedSchema				String of serialized schema that needed to be deserialize
*
* RETURN :
*            	Type:   Schema                   	Schema that is deserialized from string.
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-03-23	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Bug fixed
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
Schema *deserializeSchema (char *serializedSchema) {
	Schema *schemaResult = (Schema *)malloc(sizeof(Schema));
	char schemaData[strlen(serializedSchema)];
	char* tokenPointer;
	strcpy(schemaData, serializedSchema);

	//Get first integer (numAttr)
	char *tmpStr = strtok_r(schemaData, "<", &tokenPointer);
	tmpStr = strtok_r(NULL, ">", &tokenPointer);
	schemaResult->numAttr = atoi(tmpStr);

	schemaResult->attrNames = (char **)malloc(sizeof(char*)*schemaResult->numAttr);
	schemaResult->dataTypes = (DataType *)malloc(sizeof(DataType) * schemaResult->numAttr);
	schemaResult->typeLength = (int *)malloc(sizeof(int) * schemaResult->numAttr);
	int i;
	tmpStr = strtok_r(NULL, "(", &tokenPointer);
	for (i = 0; i < schemaResult->numAttr; i++)
	{
		tmpStr = strtok_r(NULL, ": ", &tokenPointer);
		//Put attrName into schema
		schemaResult->attrNames[i] = (char *)calloc(strlen(tmpStr), sizeof(char));
		strcpy(schemaResult->attrNames[i], tmpStr);
		//Get data type
		//If it is a last attr
		if (i == (schemaResult->numAttr) - 1)
		{
			//Cut the ) out
			tmpStr = strtok_r(NULL, ") ", &tokenPointer);
		}
		else
		{
			//If it not the last one go to next attr
			tmpStr = strtok_r(NULL, ", ", &tokenPointer);
		}
		if (strcmp(tmpStr, "INT") == 0)
		{

			schemaResult->dataTypes[i] = DT_INT;
			schemaResult->typeLength[i] = 0;

		}
		else if (strcmp(tmpStr, "FLOAT") == 0)
		{
			schemaResult->dataTypes[i] = DT_FLOAT;
			schemaResult->typeLength[i] = 0;

		}
		else if (strcmp(tmpStr, "BOOL") == 0)
		{
			schemaResult->dataTypes[i] = DT_BOOL;
			schemaResult->typeLength[i] = 0;

		}
		else
		{
			//It is String
			schemaResult->dataTypes[i] = DT_STRING;
			char *tokenPointer2;
			char *token = strtok_r(tmpStr, "[", &tokenPointer2);
			token = strtok_r(NULL, "]", &tokenPointer2);
			schemaResult->typeLength[i] = atoi(token);

		}
	}
	//Check for key
	int keySize = 0;
	char* keyAttr[schemaResult->numAttr];
	tmpStr = strtok_r(NULL, "(", &tokenPointer);
	if (tmpStr != NULL) {
		tmpStr = strtok_r(NULL, ")", &tokenPointer);
		char *tokenPointer2;
		//Inside key()
		char *tmpKey = strtok_r(tmpStr, ", ", &tokenPointer2);
		//There is only one key
		if (tmpKey == NULL)
		{
			keyAttr[keySize] = (char *)malloc(strlen(tmpStr) * sizeof(char));
			strcpy(keyAttr[keySize], tmpStr);
			keySize++;
		}
		else {
			while (tmpKey != NULL)
			{
				keyAttr[keySize] = (char *)malloc(strlen(tmpKey) * sizeof(char));
				strcpy(keyAttr[keySize], tmpKey);
				keySize++;
				tmpKey = strtok_r(NULL, ", ", &tokenPointer2);
			}
		}
	}
	if (keySize > 0) {
		schemaResult->keyAttrs = (int *)malloc(sizeof(int) * keySize);
		schemaResult->keySize = keySize;
		int j;
		for (j = 0; j < keySize; j++)
		{
			int z;
			for (z = 0; z < schemaResult->numAttr; z++)
			{
				if (strcmp(schemaResult->attrNames[z], keyAttr[j]) == 0)
				{
					schemaResult->keyAttrs[j] = z;
				}
			}
		}
	}

	return schemaResult;
}

/*******************************************************************
* NAME :            RC openTable (RM_TableData *rel, char *name)
*
* DESCRIPTION :     Opens the given table (previously created) and stores relevant data on memory
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients will use to interact with the table
*				char *name 						Name of the table to be opened
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Table opened successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC openTable (RM_TableData *rel, char *name) {
	// Declaring variables
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));

	// Initialize the buffer pool for the table with a 3-frame FIFO organization
	initBufferPool(bm, name, 3, RS_FIFO, NULL);

	// Pin the first page with the relevant information and retrieve the data
	pinPage(bm, pageHandle, 0);
	char *serializedSchema = pageHandle->data;

	// Deserialize the information about the schema
	Schema *deserializedSchema = deserializeSchema(serializedSchema);

	// Store the relevant information on the Struct RM_TableData
	rel->name = name;
	rel->schema = deserializedSchema;
	rel->mgmtData = bm;

	// Free memory, unpin and return the Return Code
	free(pageHandle);
	unpinPage(bm, pageHandle);
	return RC_OK;
}

/*******************************************************************
* NAME :            RC closeTable (RM_TableData *rel)
*
* DESCRIPTION :     Closes the given table (previously opened) and frees the memory
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients  use to interact with the table
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Table closed successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC closeTable (RM_TableData *rel) {
	// Retrieve the buffer pool information and shutdown it
	BM_BufferPool *bm = (BM_BufferPool *)rel->mgmtData;
	shutdownBufferPool(bm);

	// Free the allocated memory
	free(rel->schema->attrNames);
	free(rel->schema->dataTypes);
	free(rel->schema->typeLength);
	free(rel->schema->keyAttrs);
	free(rel->schema);
	free(rel->mgmtData);

	return RC_OK;
}

/*******************************************************************
* NAME :            RC deleteTable (char *name)
*
* DESCRIPTION :     Deletes the given table (previously created)
*
* PARAMETERS:
*            	char *name 						Name of the table to be deleted
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Table deleted successfully
*						RC_TABLE_NOT_FOUND		Table not deleted because it was not found on disk
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC deleteTable (char *name) {
	// Check if the table was removed successfully
	int removed = remove(name);
	if (removed != 0) {
		// If not, return table not found code
		return RC_TABLE_NOT_FOUND;
	}
	return RC_OK;
}

/*******************************************************************
* NAME :            int getNumTuples (RM_TableData *rel)
*
* DESCRIPTION :     Get number of tuples
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients use to interact with the table
*
* RETURN :
*            	Type:   int                   	Number of tuples
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-23	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/

int getNumTuples (RM_TableData *rel) {
	BM_BufferPool *bm = (BM_BufferPool *) rel->mgmtData;
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	SM_FileHandle fileHandle;
	openPageFile(pageFile, &fileHandle);

	//Start with block#1 (#0 is a table schema)
	int blockNum = 1;
	int totalRecord = 0;

	while (blockNum < fileHandle.totalNumPages)
	{
		//Pin the page and count
		pinPage(bm, pageHandle, blockNum);

		int i;
		for (i = 0; i < PAGE_SIZE; i++)
		{
			if (pageHandle->data[i] == '|')
				totalRecord++;
		}
		unpinPage(bm, pageHandle);
		//Read next block
		blockNum++;
	}

	return totalRecord;
}


/************************************************************
 *               HANDLING RECORDS IN A TABLE                *
 ************************************************************/

/*******************************************************************
* NAME :            RC insertRecord (RM_TableData *rel, Record *record)
*
* DESCRIPTION :     Inserts the given record on the table specified. To do so, it iterates through the file trying to
*					calculate a page number and slot number where the record can be stored.
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients use to interact with the table
*				Record *record 					Record to be stored
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record stored successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-22	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC insertRecord (RM_TableData *rel, Record *record) {
	// Declaring variables
	BM_BufferPool *bm = (BM_BufferPool *) rel->mgmtData;
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	SM_FileHandle fileHandle;
	PageNumber pageNum;
	int slotNum = 0;
	int pageLength = 0;

	// Get the total number of pages
	openPageFile(pageFile, &fileHandle);
	int totalNumPages = fileHandle.totalNumPages;
	closePageFile(&fileHandle);

	// Obtain the record size and add 3 because of | , , separators
	int recordSize = getRecordSize(rel->schema) + 3;

	// Start with the second page because the first one contains the schema information
	pageNum = 1;

	// Iterate through the pages on the file
	while (totalNumPages > pageNum) {
		// Pin the current page and get its data
		pinPage(bm, pageHandle, pageNum);
		char *pageData = pageHandle->data;

		// Calculate the length of the data already written on the page
		pageLength = strlen(pageData);

		// The remaining space will be the size of the page minus the information already written
		int remainingSpace = PAGE_SIZE - pageLength;

		// Check for empty slots inside the current page
		if (recordSize < remainingSpace) {
			// If there is enough space on the file, get the slot number and unpin the page
			slotNum = pageLength / recordSize;
			unpinPage(bm, pageHandle);
			// Stop the iteration: suitable slot and page found
			break;
		}

		// Unpin the page if there are no empty slots
		unpinPage(bm, pageHandle);

		// Iterate to next page
		pageNum++;
	}

	// Pin the resulting page to insert the record
	pinPage(bm, pageHandle, pageNum);

	// Get the pointer to the pinned page
	char *dataPointer = pageHandle->data;

	// Calculate the length of the data stored on that page
	pageLength = strlen(dataPointer);

	// Create a pointer to the position where we are inserting the record
	char *recordPointer = pageLength + dataPointer;

	// Copy the record information on the calculated pointer
	strcpy(recordPointer, record->data);

	// Mark the page dirty and unpin it
	markDirty(bm, pageHandle);
	unpinPage(bm, pageHandle);

	// Assign struct values
	RID recordID;
	recordID.page = pageNum;
	recordID.slot = slotNum;
	record->id = recordID;

	return RC_OK;
}

/*******************************************************************
* NAME :            RC deleteRecord (RM_TableData *rel, RID id)
*
* DESCRIPTION :     Deletes a record identified by an id in the given table. We have not implemented the tombstone
* 					functionality, so a simple Return Code is enough
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients use to interact with the table
*				RID id 							ID of the record to be deleted
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record deleted successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-22	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC deleteRecord (RM_TableData *rel, RID id) {
	return RC_OK;
}

/*******************************************************************
* NAME :            RC updateRecord (RM_TableData *rel, Record *record)
*
* DESCRIPTION :     Updates the given record on the table specified. It uses the page and slot
*					number to locate the record and then saves the new information in the file.
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients use to interact with the table
*				Record *record 					Record to be updated
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record stored successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-22	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC updateRecord (RM_TableData *rel, Record *record) {
	// Declaring variables
	BM_BufferPool *bm = (BM_BufferPool *) rel->mgmtData;
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	PageNumber pageNum;
	RID recordID;
	int slotNum;

	// Retrieve record information
	recordID = record->id;
	pageNum = recordID.page;
	slotNum = recordID.slot;

	// Obtain the record size and add 3 because of | , , separators
	int recordSize = getRecordSize(rel->schema) + 3;

	// Pin the corresponding page and get the data
	pinPage(bm, pageHandle, pageNum);
	char *dataPointer = pageHandle->data;

	// Define the pointer where the record will be updated
	char *recordPointer = recordSize * slotNum + dataPointer;

	// Copy the updated record in the position calculated in the pointer
	strncpy(recordPointer, record->data, recordSize);

	// Mark dirty, unpin and return the Return Code
	markDirty(bm, pageHandle);
	unpinPage(bm, pageHandle);

	return RC_OK;
}

/*******************************************************************
* NAME :            RC getRecord (RM_TableData *rel, RID id, Record *record)
*
* DESCRIPTION :     Gets the record indicated by the ID in the parameters from the table
*					specified and then saves it in the memory allocated for *record.
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients use to interact with the table
*				Record *record 					Pointer to the record retrieved
*				RID id 							ID of the record to be retrieved
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record stored successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-22	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC getRecord (RM_TableData *rel, RID id, Record *record) {
	// Declaring variables
	BM_BufferPool *bm = (BM_BufferPool *) rel->mgmtData;
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	PageNumber pageNum;
	RID recordID;
	int slotNum;

	// Retrieve record information
	recordID = id;
	pageNum = recordID.page;
	slotNum = recordID.slot;

	// Obtain the record size and add 3 because of | , , separators
	int recordSize = getRecordSize(rel->schema) + 3;

	// Pin the corresponding page and get the data
	pinPage(bm, pageHandle, pageNum);
	char *dataPointer = pageHandle->data;

	// Define the pointer of the record to be retrieved
	char *recordPointer = recordSize * slotNum + dataPointer;

	// Copy the record in the position calculated in the pointer
	strncpy(record->data, recordPointer, recordSize);

	// Unpin page and return the RC
	unpinPage(bm, pageHandle);

	return RC_OK;
}

/************************************************************
 *                          SCANS                           *
 ************************************************************/

/*******************************************************************
* NAME :            RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
*
* DESCRIPTION :     Gets the record indicated by the ID in the parameters from the table
*					specified and then saves it in the memory allocated for *record.
*
* PARAMETERS:
*            	RM_TableData *rel 				Struct that the clients use to interact with the table
*				RM_ScanHandle *scan 			Struct that contains essential data to interact with the scanning functions
*				Expr *cond 						Expression to determine which tuples match
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record stored successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-23	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond) {
	// Declaring variables
	SM_FileHandle fileHandle;
	ScanInfo *scanInfo = (ScanInfo *) malloc (sizeof(ScanInfo));

	// Get the total number of pages
	openPageFile(pageFile, &fileHandle);
	int totalNumPages = fileHandle.totalNumPages;
	closePageFile(&fileHandle);

	// Get the record size and calculate the maximum number of slots
	int recordSize = getRecordSize(rel->schema);
	int totalNumSlots = PAGE_SIZE / recordSize;

	// Fill in the Scan Information global variable
	scanInfo->currentSlot = 0;
	scanInfo->currentPage = 1;
	scanInfo->totalNumPages = totalNumPages;
	scanInfo->totalNumSlots = totalNumSlots;
	scanInfo->condition = cond;

	// Fill in the struct RM_ScanHandle
	scan->rel = rel;
	scan->mgmtData = scanInfo;

	return RC_OK;
}

/*******************************************************************
* NAME :            RC next (RM_ScanHandle *scan, Record *record)
*
* DESCRIPTION :     Returns the next record that fulfills the scan condition in a recursive way.
*
* PARAMETERS:
*				RM_ScanHandle *scan 			Struct that contains essential data to interact with the scanning functions
*				Record *record 					Pointer to the record retrieved
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record stored successfully
*						RC_RM_NO_MORE_TUPLES	No more tuples that match the condition
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-23	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC next (RM_ScanHandle *scan, Record *record) {
	// Declaring variables
	ScanInfo *scanInfo;
	Value *value;

	// Retrieve the scan information saved during the startScan function
	scanInfo = scan->mgmtData;

	// Fill in the Record Information
	record->id.page = scanInfo->currentPage;
	record->id.slot = scanInfo->currentSlot;

	// Get the current record pointed by the current page and slot information
	getRecord(scan->rel, record->id, record);

	// Check if there are more tuples in the file
	if (scanInfo->currentPage == (scanInfo->totalNumPages) - 1  && scanInfo->currentSlot == (scanInfo->totalNumSlots) - 1) {
		// If it is the last tuple, return the RC indicating that there are no more tuples
		return RC_RM_NO_MORE_TUPLES;
	}

	// Evaluate the condition and check if there are matching tuples
	evalExpr(record, scan->rel->schema, scanInfo->condition, &value);

	if (scanInfo->currentSlot != scanInfo->totalNumSlots) {
		// If there are more slots, go to the next one
		(scanInfo->currentSlot)++;
	} else {
		// If not, go to the next page and start by the first slot
		scanInfo->currentSlot = 0;
		(scanInfo->currentPage)++;
	}

	// Update the scan information for the next execution of this function
	scan->mgmtData = scanInfo;

	if (value->v.boolV == 1) {
		// If the tuple matches, then return RC_OK
		return RC_OK;
	} else {
		// If not, calculate the next tuple again
		return next(scan, record);
	}
}

/*******************************************************************
* NAME :            RC closeScan (RM_ScanHandle *scan)
*
* DESCRIPTION :     Closes the scan
*
* PARAMETERS:
*				RM_ScanHandle *scan 			Struct that contains essential data to interact with the scanning functions
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Record stored successfully
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-23	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC closeScan (RM_ScanHandle *scan) {
	return RC_OK;
}

/************************************************************
 *                   DEALING WITH SCHEMAS                   *
 ************************************************************/

/*******************************************************************
* NAME :            int getRecordSize (Schema *schema)
*
* DESCRIPTION :     Gets the size of a record in the given schema.
*
* PARAMETERS:
*				Schema *schema 					Schema that will be used in the table
* RETURN :
*            	Type:   int                   	Size of the record in bytes
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-22	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-31     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
int getRecordSize (Schema *schema) {
	// Declaring variables
	int size = 0;
	int i;
	DataType *dataTypes = schema->dataTypes;
	int numAttr = schema->numAttr;

	// Iterate the number of attributes in the schema to calculate the size
	for (i = 0; i < numAttr; i++) {
		int temp = 0;
		switch (dataTypes[i]) {
		case DT_INT:
			temp += sizeof(int);
			break;
		case DT_STRING:
			temp += schema->typeLength[i];
			break;
		case DT_FLOAT:
			temp += sizeof(float);
			break;
		case DT_BOOL:
			temp += sizeof(bool);
			break;
		default:
			break;
		}
		size += temp;
	}
	// Return final size after all attributes have been checked
	return size;
}

/*******************************************************************
* NAME :            Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
*
* DESCRIPTION :     Create a schema and fill schema with assigned parameter
*
* PARAMETERS:
*            	int numAttr 				Number of attribute
*            	char **attrNames			Pointer of attribute name
*            	DataType *dataTypes		Data type of its column
*            	int *typeLength			Length data
*            	int keySize					Size of key
*            	int *keys					Pointer to keys
*
*
* RETURN :
*            	Type:   Schema                   	Created schema
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/

Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys) {
	Schema *schema = (Schema*) malloc (sizeof(Schema));

	schema->numAttr = numAttr;
	schema->attrNames = attrNames;
	schema->dataTypes = dataTypes;
	schema->typeLength = typeLength;
	schema->keySize = keySize;
	schema->keyAttrs = keys;
	return schema;
}


/*******************************************************************
* NAME :            RC freeSchema (Schema *schema)
*
* DESCRIPTION :     free the memory of given schema
*
* PARAMETERS:
*            	Schema *schema 				Schema wanted to be free
*
*
* RETURN :
*            	Type:   RC                   	Return Code:
* 				Values: RC_OK                  Schema free successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC freeSchema (Schema *schema) {
	free(schema);
	return RC_OK;
}


/************************************************************
 *       DEALING WITH RECORDS AND ATTRIBUTE VALUES          *
 ************************************************************/

/*******************************************************************
* NAME :            RC createRecord (Record **record, Schema *schema)
*
* DESCRIPTION :     create record
*
* PARAMETERS:
*            	Record **record			Pointer to a record
* 				Schema *schema				Schema of the record
*
*
* RETURN :
*            	Type:   RC                   	Return Code:
* 				Values: RC_OK                  Record create successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC createRecord (Record **record, Schema *schema) {
	int size = getRecordSize(schema);
	Record *r = (Record*) malloc (sizeof(Record));
	r->data = (char*) calloc(size, sizeof(char));
	*record = r;
	return RC_OK;
}

/*******************************************************************
* NAME :            RC freeRecord (Record *record)
*
* DESCRIPTION :     free the memory of given revcord
*
* PARAMETERS:
*            	Record *record 				Record wanted to be free
*
*
* RETURN :
*            	Type:   RC                   	Return Code:
* 				Values: RC_OK                  record free successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-21	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC freeRecord (Record *record) {
	free(record->data);
	free(record);
	return RC_OK;
}

/*******************************************************************
* NAME :            RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
*
* DESCRIPTION :     get specific attribute from record
*
* PARAMETERS:
*            	Record *record 				Record wanted to be read
* 				Schema *schema					Schema of the record
* 				int attrNum						Position of the attribute needed to be read
* 				Value **value					Output
*
*
* RETURN :
*            	Type:   RC                   	Return Code:
* 				Values: RC_OK                  Read atrribute successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-23	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-03-30	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Fixed Bugs
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC getAttr (Record *record, Schema *schema, int attrNum, Value **value) {
	Value *val = (Value*)malloc(sizeof(Value));
	int offset =  0;
	int i;
	for (i = 0; i < attrNum; i++) {
		if (schema->dataTypes[i] == DT_INT) {
			offset += sizeof(int);
		}
		else if (schema->dataTypes[i] == DT_FLOAT) {
			offset += sizeof(float);
		}
		else if (schema->dataTypes[i] == DT_BOOL) {
			offset += sizeof(bool);
		}
		else if (schema->dataTypes[i] == DT_STRING) {
			offset += schema->typeLength[i];
		}
	}
	offset += attrNum + 1; // Separator Charactor
	char *output;
	if (schema->dataTypes[attrNum] == DT_INT) {
		output = (char *)calloc(sizeof(int), sizeof(char));
		strcpy(output, record->data + offset);
		val->dt = DT_INT;
		val->v.intV = atoi(output);
	}
	else if (schema->dataTypes[attrNum] == DT_FLOAT) {
		output = (char *)calloc(sizeof(float), sizeof(char));
		strcpy(output, record->data + offset);
		val->dt = DT_FLOAT;
		val->v.floatV = (float) * output;
	}
	else if (schema->dataTypes[attrNum] == DT_BOOL) {
		output = (char *)calloc(sizeof(bool), sizeof(char));
		strcpy(output, record->data + offset);
		val->dt = DT_BOOL;
		val->v.boolV = (bool) * output;
	}
	else if (schema->dataTypes[attrNum] == DT_STRING) {
		output = (char *)calloc(schema->typeLength[attrNum], sizeof(char));
		strncpy(output, record->data + offset, schema->typeLength[attrNum]);
		val->dt = DT_STRING;
		val->v.stringV = output;
	}
	*value = val;
	return RC_OK;
}

/*******************************************************************
* NAME :            RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
*
* DESCRIPTION :     set specific attribute from record
*
* PARAMETERS:
*            	Record *record 				Record wanted to be set
* 				Schema *schema					Schema of the record
* 				int attrNum						Position of the attribute needed to be set
* 				Value *value					value address needed to be set
*
*
* RETURN :
*            	Type:   RC                   	Return Code:
* 				Values: RC_OK                  Set atrribute successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-03-23	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-03-30	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Fixed Bugs
*            2016-02-31     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC setAttr (Record *record, Schema *schema, int attrNum, Value *value) {
	int offset = 0;
	int i;
	//Offset for extra space charactor
	offset += attrNum + 1;

	//Offset for data
	for (i = 0; i < attrNum; i++) {
		if (schema->dataTypes[i] == DT_INT)
		{
			offset += sizeof(int);
		}
		else if (schema->dataTypes[i] == DT_FLOAT)
		{
			offset += sizeof(float);
		}
		else if (schema->dataTypes[i] == DT_BOOL)
		{
			offset += sizeof(bool);
		}
		else if (schema->dataTypes[i] == DT_STRING)
		{
			offset += schema->typeLength[i];
		}
	}
	char *output = record->data;
	//Attr is the first one
	if (attrNum == 0) {
		output[0] = '|';
		output++;
	}
	else {
		output += offset;
		(output - 1)[0] = ',';
	}

	if (value->dt == DT_INT) {
		sprintf(output, "%04d", value->v.intV);
	}
	else if (value->dt == DT_FLOAT) {
		sprintf(output, "%04f", value->v.floatV);
	}
	else if (value->dt == DT_BOOL) {
		sprintf(output, "%i", value->v.boolV);
	}
	else if (value->dt == DT_STRING) {
		sprintf(output, "%s", value->v.stringV);
	}
	return RC_OK;
}