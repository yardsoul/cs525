#include "btree_mgr.h"
#include "buffer_mgr.h"
#include "record_mgr.h"
#include "storage_mgr.h"
#include "tables.h"

struct BTreeInfo
{
	struct Value val;
	struct RID rid;
} BTreeInfo;

BTreeInfo **bTree;

int nodeNum;
int nextNode;

// init and shutdown index manager
RC initIndexManager (void *mgmtData) {
	bTree = (BTreeInfo **) malloc (sizeof(BTreeInfo *) * 50);
	return RC_OK;
}

RC shutdownIndexManager () {
	free(bTree);
	return RC_OK;
}

// create, destroy, open, and close an btree index
RC createBtree (char *idxId, DataType keyType, int n) {
	BTreeHandle *btHandle;
	btHandle = (BTreeHandle *) malloc (sizeof(BTreeHandle) * 3);

	btHandle->keyType = keyType;
	btHandle->idxId = idxId;

	nodeNum = 0;
	nextNode = 0;

	return RC_OK;
}

RC openBtree (BTreeHandle **tree, char *idxId) {
	*tree = (BTreeHandle *) malloc (sizeof(BTreeHandle) * 3);
	(*tree)->idxId = (char *) malloc (sizeof(char) * (strlen(idxId) + 1));
	(*tree)->idxId = idxId;

	return RC_OK;
}

RC closeBtree (BTreeHandle *tree) {
	free(tree);
	return RC_OK;
}

RC deleteBtree (char *idxId) {
	nodeNum = 0;
	nextNode = 0;
	free(idxId);

	return RC_OK;
}

// access information about a b-tree
RC getNumNodes (BTreeHandle *tree, int *result) {
	int counter = 0;
	int result = 0;
	int i;
	int j;

	for (i = 0; i<nodeNum; i++) {
		for (j = i-1; j>=0; j--) {
			if (bTree[j]->rid.page == bTree[i]->rid.page) {
				counter++;
				break;
			}
		}
	}

	*result = nodeNum-counter;

	return RC_OK;

}

RC getNumEntries (BTreeHandle *tree, int *result) {
	*result = nodeNum;
	return RC_OK;
}

RC getKeyType (BTreeHandle *tree, DataType *result) {
	*result = tree->keyType;
	return RC_OK;
}

// index access
RC findKey (BTreeHandle *tree, Value *key, RID *result) {
	
}

RC insertKey (BTreeHandle *tree, Value *key, RID rid) {

}

RC deleteKey (BTreeHandle *tree, Value *key) {

}

RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle) {

}

RC nextEntry (BT_ScanHandle *handle, RID *result) {

}

RC closeTreeScan (BT_ScanHandle *handle) {

}

// debug and test functions
char *printTree (BTreeHandle *tree) {

}