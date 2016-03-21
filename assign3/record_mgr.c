#include "record_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"


RC initRecordManager (void *mgmtData) {
	return RC_OK;
}

RC shutdownRecordManager () {
	return RC_OK;
}

RC createTable (char *name, Schema *schema) {
	// Create the file
	createPageFile(name);

	SM_FileHandle fileHandle;

	openPageFile(name, &fileHandle);


	ensureCapacity(1, &fileHandle);

	char *schemaToString = serializeSchema(schema);



	writeBlock(0, &fileHandle, schemaToString);

	closePageFile(&fileHandle);
	int t;
	scanf("%d", t);
	return RC_OK;
}

RC openTable (RM_TableData *rel, char *name) {
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));

	initBufferPool(bm, name, 3, RS_FIFO, NULL);

	pinPage(bm, pageHandle, 0);

	serializedSchema = pageHandle->data;
	deserializedSchema = deserializeSchema(serializedSchema);

	rel->name = name;
	rel->schema = deserializedSchema;
	rel->mgmtData = bm;

	free(pageHandle);

	return RC_OK;

}


Schema *deserializeSchema (char *serializedSchema) {
	Schema *deserializedSchema = (Schema *) malloc (sizeof(Schema));

}


RC closeTable (RM_TableData *rel) {

}

RC deleteTable (char *name) {

}

int getNumTuples (RM_TableData *rel) {

}




// handling records in a table
RC insertRecord (RM_TableData *rel, Record *record) {

}

RC deleteRecord (RM_TableData *rel, RID id) {

}

RC updateRecord (RM_TableData *rel, Record *record) {

}

RC getRecord (RM_TableData *rel, RID id, Record *record) {

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

}


// dealing with records and attribute values
RC createRecord (Record **record, Schema *schema) {

}

RC freeRecord (Record *record) {

}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value) {

}

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value) {

}
