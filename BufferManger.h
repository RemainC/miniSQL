#ifndef BufferManager_h
#define BufferManager_h
#include "Record.h"
#include <iostream>
#include <memory.h>
#include <map>

#define PAGE_SIZE 4096
#define MAX_PAGE_NUM 10240

using namespace std;

class page {
public:
	int pageId;
	bool dirty;
	int usedLength;
	string tableName;
	char fileName[64];
	int rowSize;
	char pageData[PAGE_SIZE];

	page() {};
	page(string tName) :tableName(tName) {};
	page(string tName, int rSize);
	bool loadPage();
	bool writeBackPage();
	bool deleteFromPage(int offset);
	bool insertIntoPage(char* newData);
};

class memoryPool {
public:
	string tableName;
	char fileName[64];
	int rowSize;
	int usedPage;
	tableInfo tInfo;
	bool dirty;
	bool isFull;

	page* pagePool[MAX_PAGE_NUM];

	memoryPool(string tName);
	bool getTableInfo();

	bool loadBuffer();
	void dropBuffer();
	bool writeBackBuffer();
	bool writeBackBuffer(int pageId);

	bool insertIntoBuffer(char* newData);
	bool insertIntoBuffer(vector<class element>elements);
	bool deleteFromBuffer(int offset);
	void seekLazyDelete();

	bool deleteTable();
};

class bufferManager {
public:
	map<string, memoryPool*> tableBufferMap;

	void createTable(string tableName, Record record);
	void newTableBuffer(string tableName);
	void dropTableBuffer(string tableName);

	bool getTableInfo(string tableName);
	bool loadTable(string tableName);
	bool writeBackTable(string tableName);

	char* queryTable(string tableName);
	char* queryTable(string table, int offset);

	bool insertIntoTable(string tableName, char* newData);
	bool insertIntoTable(string tableName, vector<class element>elements);

	bool deleteFromTable(string tableName, int offset);
	bool deleteTable(string tableName);
};

#endif