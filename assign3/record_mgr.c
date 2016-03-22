#include "record_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include <stdlib.h>
#include <string.h>

int numTuples = 0;
int numSlots = 0;
int slotSize = 0;
char pageFile[100];


RC initRecordManager (void *mgmtData) {
	return RC_OK;
}

RC shutdownRecordManager () {
	return RC_OK;
}

RC createTable (char *name, Schema *schema) {
	strcpy(pageFile, name);

	// Create the file
	createPageFile(name);

	SM_FileHandle fileHandle;

	openPageFile(name, &fileHandle);



	ensureCapacity(1, &fileHandle);

	char *schemaToString = serializeSchema(schema);



	writeBlock(0, &fileHandle, schemaToString);


	// TODO: Functions to change global variables


	closePageFile(&fileHandle);
	// int t;
	// scanf("%d", t);
	return RC_OK;
}

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
	int i;
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


RC openTable (RM_TableData *rel, char *name) {
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));

	initBufferPool(bm, name, 3, RS_FIFO, NULL);

	pinPage(bm, pageHandle, 0);

	char *serializedSchema = pageHandle->data;
	Schema *deserializedSchema = deserializeSchema(serializedSchema);

	rel->name = name;
	rel->schema = deserializedSchema;
	rel->mgmtData = bm;

	free(pageHandle);

	return RC_OK;

}


RC closeTable (RM_TableData *rel) {
	BM_BufferPool *bm = (BM_BufferPool *)rel->mgmtData;
	shutdownBufferPool(bm);
	free(rel->schema->attrNames);
	free(rel->schema->dataTypes);
	free(rel->schema->typeLength);
	free(rel->schema->keyAttrs);
	free(rel->schema);
	free(rel->mgmtData);

	return RC_OK;
}

RC deleteTable (char *name) {
	int removed = remove(name);
	if (removed != 0) {
		return RC_TABLE_NOT_FOUND;
	}
	return RC_OK;
}

int getNumTuples (RM_TableData *rel) {

}




// handling records in a table
RC insertRecord (RM_TableData *rel, Record *record) {
	// Declaring variables
	BM_BufferPool *bm = (BM_BufferPool *) rel->mgmtData;
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	SM_FileHandle *fileHandle;
	PageNumber pageNum;
	int slotNum = 0;
	int pageLength = 0;


	// Get the total number of pages
	openPageFile(pageFile, &fileHandle);
	int totalNumPages = fileHandle->totalNumPages;
	closePageFile(&fileHandle);

	int recordSize = getRecordSize(rel->schema);

	pageNum = 1;

	while (pageNum < totalNumPages) {
		// Pin the current page
		pinPage(bm, pageHandle, pageNum);

		// Get the information of the file
		char *pageData = pageHandle->data;

		// Calculate the length of the page
		pageLength = strlen(pageData);

		int remainingSpace = PAGE_SIZE - pageLength;

		// Check for empty slots inside the current page
		if (recordSize < remainingSpace) {
			slotNum = pageLength / recordSize;
			unpinPage(bm, pageHandle);
			break;
		}

		// Unpin the page if there is no empty slots
		unpinPage(bm, pageHandle);

		// Iterate to next page
		pageNum++;
	}

	// If there is no space left, we append an empty block
	if (slotNum == 0) {
		pinPage(bm, pageHandle, pageNum + 1);
		unpinPage(bm, pageHandle);
	}

	// Pin the resulting page to insert the record
	pinPage(bm, pageHandle, pageNum);

	// Get the pointer to the pinned page
	char *dataPointer = pageHandle->data;

	// Calculate the length of the page
	pageLength = strlen(dataPointer);

	// Create a pointer to the position where we are inserting the record
	char *recordPointer = pageLength + dataPointer;

	strcpy(recordPointer, record->data);

	markDirty(bm, pageHandle);

	unpinPage(bm, pageHandle);

	// Assign struct values

	RID recordID;

	recordID.page = pageNum;
	recordID.slot = slotNum;

	record->id = recordID;

	return RC_OK;
}

RC deleteRecord (RM_TableData *rel, RID id) {
	return RC_OK;
}

RC updateRecord (RM_TableData *rel, Record *record) {
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

	// Get the size of the record to be updated
	int recordSize = getRecordSize(rel->schema);

	// Pin the corresponding page and get the data
	pinPage(bm, pageHandle, pageNum);
	char *dataPointer = pageHandle->data;

	// Define the pointer where the record will be updated
	char *recordPointer = recordSize * slotNum + dataPointer;

	// Copy the updated record in the position
	strcpy(recordPointer, record->data);

	markDirty(bm, pageHandle);

	unpinPage(bm, pageHandle);

	return RC_OK;
}

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

	// Get the size of the record to be updated
	int recordSize = getRecordSize(rel->schema);

	// Pin the corresponding page and get the data
	pinPage(bm, pageHandle, pageNum);
	char *dataPointer = pageHandle->data;

	// Define the pointer of the record to be read
	char *recordPointer = recordSize * slotNum + dataPointer;

	strcpy(recordPointer, record->data);
	
	unpinPage(bm, pageHandle);

	return RC_OK;
}


// scans
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond) {

}

RC next (RM_ScanHandle *scan, Record *record) {

}

RC closeScan (RM_ScanHandle *scan) {

}


// dealing with schemas
int getRecordSize (Schema *schema) {
	int size = 0;
	int temp = 0;
	int i;

	DataType *dataTypes = schema->dataTypes;
	int numAttr = schema->numAttr;

	for (i = 0; i < numAttr; i++) {
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
	return size;
}

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

RC freeSchema (Schema *schema) {
	free(schema);
	return RC_OK;
}


// dealing with records and attribute values
RC createRecord (Record **record, Schema *schema) {

}

RC freeRecord (Record *record) {
	free(record->data);
	free(record);
	return RC_OK;
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value) {

}

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value) {

}
