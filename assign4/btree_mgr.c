#include <string.h>
#include <stdlib.h>

#include "btree_mgr.h"
#include "buffer_mgr.h"
#include "record_mgr.h"
#include "storage_mgr.h"
#include "tables.h"

typedef struct BTreeInfo
{
	struct Value val;
	struct RID rid;
} BTreeInfo;

BTreeInfo **bTree;

int nodeNum;
int nextNode;

int searchKey (BTreeHandle *tree, Value *key) {
	int i;
	for (i = 0; i < nodeNum; i++) {
		if (bTree[i]->val.dt == key->dt
		        && ((bTree[i]->val.dt == DT_INT &&  bTree[i]->val.v.intV == key->v.intV)
		            || (bTree[i]->val.dt == DT_FLOAT && bTree[i]->val.v.floatV == key->v.floatV)
		            || (bTree[i]->val.dt == DT_STRING && strcmp(bTree[i]->val.v.stringV, key->v.stringV) == 0))) {
			return i;
		}
	}

	return -1;
}

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
	int i;
	int j;

	for (i = 0; i < nodeNum; i++) {
		for (j = i - 1; j >= 0; j--) {
			if (bTree[j]->rid.page == bTree[i]->rid.page) {
				counter++;
				break;
			}
		}
	}

	*result = nodeNum - counter;

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
	int i = searchKey(tree, key);
	if (i != -1) {
		result->page = bTree[i]->rid.page;
		result->slot = bTree[i]->rid.slot;
		return RC_OK;
	}
	return RC_IM_KEY_NOT_FOUND;
}

RC insertKey (BTreeHandle *tree, Value *key, RID rid) {
	bTree[nodeNum] = (BTreeInfo *)malloc(sizeof(BTreeInfo));

	if (searchKey(tree, key) == -1) {
		bTree[nodeNum]->rid.page = rid.page;
		bTree[nodeNum]->rid.slot = rid.slot;
		bTree[nodeNum]->val.dt = key->dt;
		if (key->dt == DT_INT) {
			bTree[nodeNum]->val.v.intV = key->v.intV;
		}
		else if (key->dt == DT_FLOAT) {
			bTree[nodeNum]->val.v.floatV = key->v.floatV;
		}
		else if (key->dt == DT_STRING) {
			strcpy(bTree[nodeNum]->val.v.stringV, key->v.stringV);
		}
		nodeNum++;
		return RC_OK;
	}
	else {
		return RC_IM_KEY_ALREADY_EXISTS;
	}
}

RC deleteKey (BTreeHandle *tree, Value *key) {
	int index = searchKey(tree, key);
	if (index != -1) {
		int i = index;
		index++;
		for (; i < nodeNum && index < nodeNum; i++, index++) {
			bTree[i]->rid.page = bTree[index]->rid.page;
			bTree[i]->rid.slot = bTree[index]->rid.slot;
			bTree[i]->val.dt = bTree[index]->val.dt;
			if (key->dt == DT_INT) {
				bTree[i]->val.v.intV = bTree[index]->val.v.intV;
			}
			else if (key->dt == DT_FLOAT) {
				bTree[i]->val.v.floatV = bTree[index]->val.v.floatV;
			}
			else if (key->dt == DT_STRING) {
				strcpy(bTree[i]->val.v.stringV, bTree[index]->val.v.stringV);
			}
		}
		free(bTree[i]);
		return RC_OK;
	}
	return RC_IM_KEY_NOT_FOUND;
}

RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle) {
	int i;
	for (i = 0; i < nodeNum - 1; i++) {
		int index = i;
		int j;
		for (j = i + 1; j < nodeNum; j++) {
			if (bTree[j]->val.v.intV < bTree[index]->val.v.intV) {
				index = j;
			}
		}

		// BTreeInfo *tmpBTree = (BTreeInfo *) malloc (sizeof(BTreeInfo));

		// tmpBTree = bTree[i];

		Value tmpValue;
		RID tmpRID;

		tmpValue.dt = bTree[i]->val.dt;
		tmpValue.v.intV = bTree[i]->val.v.intV;
		tmpRID.page = bTree[i]->rid.page;
		tmpRID.slot = bTree[i]->rid.slot;

		bTree[i]->val.dt = bTree[j]->val.dt;
		bTree[i]->val.v.intV = bTree[j]->val.v.intV;
		bTree[i]->rid.page = bTree[j]->rid.page;
		bTree[i]->rid.slot = bTree[j]->rid.slot;

		bTree[j]->val.dt = tmpValue.dt;
		bTree[j]->val.v.intV = tmpValue.v.intV;
		bTree[j]->rid.page = tmpRID.page;
		bTree[j]->rid.slot = tmpRID.slot;

		return RC_OK;



	}
}

RC nextEntry (BT_ScanHandle *handle, RID *result) {
	if (nextNode < nodeNum) {
		result->page = bTree[nextNode]->rid.page;
		result->slot = bTree[nextNode]->rid.slot;
		nextNode++;
		return RC_OK;
	}

	return RC_IM_NO_MORE_ENTRIES;
}

RC closeTreeScan (BT_ScanHandle *handle) {
	free(handle);
	return RC_OK;
}

// debug and test functions
char *printTree (BTreeHandle *tree) {
	return tree->idxId;
}