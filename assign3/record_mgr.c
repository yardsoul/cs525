#include "record_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"

typedef struct TableInfo {
	int numTuples;
	BM_BufferPool *bm;
} TableInfo;


Schema *deserializeSchema (char *serializedSchema) {

}

// TableInfo *deserializeTableInfo (char *serializedTableInfo) {
// 	TableInfo * tInfo = (TableInfo *) malloc (sizeof(TableInfo));

// 	return tInfo;
// }

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

	// ensureCapacity(1, &fileHandle);

	// char *tableInfoToString = serializeTableInfo();

	// writeBlock(1, &fileHandle, tableInfoToString);

	closePageFile(&fileHandle);
	// int t;
	// scanf("%d", t);
	return RC_OK;
}

RC openTable (RM_TableData *rel, char *name) {
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	BM_PageHandle *pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));

	initBufferPool(bm, name, 3, RS_FIFO, NULL);

	pinPage(bm, pageHandle, 0);

	char *serializedSchema = pageHandle->data;
	Schema *deserializedSchema = deserializeSchema(serializedSchema);

	// pinPage(bm, pageHandle, 1);

	// TableInfo *tInfo;
	// char *serializedTableInfo = pageHandle->data;
	// tInfo = deserializeTableInfo(serializedTableInfo);

	// tInfo->bm = bm;

	rel->name = name;
	rel->schema = deserializedSchema;
	rel->mgmtData = bm;

	free(pageHandle);

	return RC_OK;

}




RC closeTable (RM_TableData *rel) {
	BM_BufferPool *bm = ((rel->mgmtData)->bm);
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
	// int numTuples = ((rel->mgmtData)->numTuples);
	// return numTuples;
}




// handling records in a table
RC insertRecord (RM_TableData *rel, Record *record) {
	BM_PageHandle pageHandle = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	TableInfo *tInfo = (TableInfo *) rel->mgmtData;

	PageNumber pageNum;
	int slotNum;



	free(pageHandle);
	return RC_OK;
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
