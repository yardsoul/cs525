#include "record_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "expr.h"
#include <stdlib.h>
#include <string.h>


char pageFile[100];

typedef struct ScanInfo
{
	int currentPage;
	int currentSlot;
	int totalNumPages;
	int totalNumSlots;
	Expr *condition;
} ScanInfo;



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
	schemaResult->typeLength = (int *)malloc(sizeof(int)*schemaResult->numAttr);
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


RC openTable (RM_TableData *rel, char *name) {
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	printf("open start\n");
	initBufferPool(bm, name, 3, RS_FIFO, NULL);
	printf("after initBuffer\n");
	pinPage(bm, pageHandle, 0);
	printf("after pin %s\n",pageHandle->data);
	char *serializedSchema = pageHandle->data;
	printf("berfor deserialize\n");
	Schema *deserializedSchema = deserializeSchema(serializedSchema);
	printf("before set rel\n");
	rel->name = name;
	rel->schema = deserializedSchema;
	rel->mgmtData = bm;
	printf("after set rel\n");
	free(pageHandle);
	unpinPage(bm,pageHandle);
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
	BM_BufferPool *bm = (BM_BufferPool *) rel->mgmtData;
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	SM_FileHandle fileHandle;
	openPageFile(pageFile, &fileHandle);

	//Start with block#1 (#0 is a table schema)
	int blockNum = 1;
	int totalNumPages = fileHandle.totalNumPages;
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




// handling records in a table
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

	int recordSize = getRecordSize(rel->schema) + 3; // Because of separators

	pageNum = 1;

	while (pageNum < totalNumPages) {
		// Pin the current page

		pinPage(bm, pageHandle, pageNum);


		// Get the information of the file
		char *pageData = pageHandle->data;

		// Calculate the length of the page
		pageLength = strlen(pageData);
		//printf("pageLength: %d\n", pageLength);

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
//	 if (slotNum == 0) {
//	 	pinPage(bm, pageHandle, pageNum);
//	 	unpinPage(bm, pageHandle);
//	 }

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
	recordID = record->id;
	pageNum = recordID.page;
	slotNum = recordID.slot;

	// Get the size of the record to be updated
	int recordSize = getRecordSize(rel->schema)+3;

	// Pin the corresponding page and get the data
	pinPage(bm, pageHandle, pageNum);
	char *dataPointer = pageHandle->data;

	// Define the pointer where the record will be updated
	char *recordPointer = recordSize * slotNum + dataPointer;

	// Copy the updated record in the position
	strncpy(recordPointer, record->data,recordSize);

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
	int recordSize = getRecordSize(rel->schema)+3;

	// Pin the corresponding page and get the data
	pinPage(bm, pageHandle, pageNum);
	char *dataPointer = pageHandle->data;

	// Define the pointer of the record to be read
	char *recordPointer = recordSize * slotNum + dataPointer;

	strncpy(record->data,recordPointer,recordSize);

	unpinPage(bm, pageHandle);

	return RC_OK;
}


// scans
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond) {
	// Declaring variables
	SM_FileHandle fileHandle;
	ScanInfo *scanInfo = (ScanInfo *) malloc (sizeof(ScanInfo));

	// Get the total number of pages
	openPageFile(pageFile, &fileHandle);
	int totalNumPages = fileHandle.totalNumPages;
	closePageFile(&fileHandle);

	int recordSize = getRecordSize(rel->schema);
	int totalNumSlots = PAGE_SIZE / recordSize;

	// Fill in the Scan Information
	scanInfo->currentSlot = 0;
	scanInfo->currentPage = 1;
	scanInfo->totalNumPages = totalNumPages;
	scanInfo->totalNumSlots = totalNumSlots;
	scanInfo->condition = cond;

	scan->rel = rel;
	scan->mgmtData = scanInfo;

	return RC_OK;
}

RC next (RM_ScanHandle *scan, Record *record) {
	// Declaring variables
	ScanInfo *scanInfo;
	Value *value;

	scanInfo = scan->mgmtData;



	// Fill in the Record Information
	record->id.page = scanInfo->currentPage;
	record->id.slot = scanInfo->currentSlot;

	getRecord(scan->rel, record->id, record);

	if (scanInfo->currentPage == (scanInfo->totalNumPages) - 1  && scanInfo->currentSlot == (scanInfo->totalNumSlots) - 1) {
		return RC_RM_NO_MORE_TUPLES;
	}

	evalExpr(record, scan->rel->schema, scanInfo->condition, &value);
	if (scanInfo->currentSlot != scanInfo->totalNumSlots) {
		(scanInfo->currentSlot)++;
	} else {
		scanInfo->currentSlot = 0;
		(scanInfo->currentPage)++;
	}

	scan->mgmtData = scanInfo;

	if (value->v.boolV == 1) {
		return RC_OK;
	} else {
		return next(scan, record);
	}
}

RC closeScan (RM_ScanHandle *scan) {
	return RC_OK;
}


// dealing with schemas
int getRecordSize (Schema *schema) {
	int size = 0;
	
	int i;

	DataType *dataTypes = schema->dataTypes;
	int numAttr = schema->numAttr;

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
	int size = getRecordSize(schema);
	Record *r = (Record*) malloc (sizeof(Record));
	r->data = (char*) calloc(size,sizeof(char));
	*record = r;
	return RC_OK;
}

RC freeRecord (Record *record) {
	free(record->data);
	free(record);
	return RC_OK;
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value) {
	Value *val = (Value*)malloc(sizeof(Value));
	int offset =  0;
	int i;
	for(i = 0; i< attrNum;i++){
		if(schema->dataTypes[i] == DT_INT){
			offset += sizeof(int);
		}
		else if(schema->dataTypes[i] == DT_FLOAT){
			offset += sizeof(float);
		}
		else if(schema->dataTypes[i] == DT_BOOL){
			offset += sizeof(bool);
		}
		else if(schema->dataTypes[i] == DT_STRING){
			offset += schema->typeLength[i];
		}
	}
	offset += attrNum+1;
	char *output;
	if(schema->dataTypes[attrNum] == DT_INT){
		output = (char *)calloc(sizeof(int),sizeof(char));
		strcpy(output,record->data+offset);
		val->dt = DT_INT;
		val->v.intV = atoi(output);
	}
	else if(schema->dataTypes[attrNum] == DT_FLOAT){
		output = (char *)calloc(sizeof(float),sizeof(char));
		strcpy(output,record->data+offset);
		val->dt = DT_FLOAT;
		val->v.floatV = (float) *output;
	}
	else if(schema->dataTypes[attrNum] == DT_BOOL){
		output = (char *)calloc(sizeof(bool),sizeof(char));
		strcpy(output,record->data+offset);
		val->dt = DT_BOOL;
		val->v.boolV = (bool) *output;
	}
	else if(schema->dataTypes[attrNum] == DT_STRING){
		output = (char *)calloc(schema->typeLength[attrNum],sizeof(char));
		strncpy(output,record->data+offset,schema->typeLength[attrNum]);
		val->dt = DT_STRING;
		val->v.stringV = output;
	}
	*value = val;
	return RC_OK;
}

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
//		while (strlen(output) != sizeof(int)) {
//			strcat(output, "0");
//		}
//		int j, k;
//		for (j = 0, k = strlen(output) - 1; j < k; j++, k--) {
//			int tmp = output[j];
//			output[j] = output[k];
//			output[k] = tmp;
//		}
	}
	else if (value->dt == DT_FLOAT) {
		sprintf(output, "%04f", value->v.floatV);
//		while (strlen(output) != sizeof(float)) {
//			strcat(output, "0");
//		}
//		int j, k;
//		for (j = 0, k = strlen(output) - 1; j < k; j++, k--) {
//			int tmp = output[j];
//			output[j] = output[k];
//			output[k] = tmp;
//		}
	}
	else if (value->dt == DT_BOOL) {
		sprintf(output, "%i", value->v.boolV);
	}
	else if (value->dt == DT_STRING) {
		sprintf(output, "%s", value->v.stringV);
	}
	return RC_OK;
}