#include "btree_mgr.h"

// init and shutdown index manager
RC initIndexManager (void *mgmtData) {

}

RC shutdownIndexManager () {

}

// create, destroy, open, and close an btree index
RC createBtree (char *idxId, DataType keyType, int n) {

}

RC openBtree (BTreeHandle **tree, char *idxId) {

}

RC closeBtree (BTreeHandle *tree) {

}

RC deleteBtree (char *idxId) {

}

// access information about a b-tree
RC getNumNodes (BTreeHandle *tree, int *result) {

}

RC getNumEntries (BTreeHandle *tree, int *result) {

}

RC getKeyType (BTreeHandle *tree, DataType *result) {

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