#ifndef IndexManager_h
#define IndexManager_h


#define INDEXPATH "/Users/remain/Downloads/new/data/index/"

#include "BufferManger.h"
#include <iostream>
#include <memory.h>
#include <map>
#include <string>

using namespace std;


typedef struct bptnp{
	element e;
	long offset;
} bpTreeNodepair;

class indexManager {
public:
	vector<struct bptnp> indexVector;
	bufferManager bfm;

	bool createFromRecord(string tablename, string elename);
	bool writeBackIndex(string tablename, string elename);
	bool deleteIndex(string tablename, string elename);
};

#endif
