#include "BufferManger.h"
#include "math.h"
//=================page====================
page::page(string tName, int rSize) {
	memset(pageData, 0, PAGE_SIZE);
	dirty = false;
	usedLength = 0;
	tableName = tName;
	strcpy(fileName, TABLEPATH);
	strcat(fileName, tableName.c_str());
	strcat(fileName,".sdat");
	rowSize = rSize;
}

bool page::loadPage() {
	int offset = pageId*PAGE_SIZE;
	FILE* fp=fopen(fileName, "rb+");
	fseek(fp, offset, SEEK_SET);
	usedLength=fread(pageData, 1, PAGE_SIZE, fp);
	fclose(fp);
	if (usedLength == 0)
		return false;
	else
		return true;
}

bool page::writeBackPage() {
	int offset = pageId*PAGE_SIZE;
	if (dirty == false)
		return false;

	FILE* fp = fopen(fileName, "rb+");
	fseek(fp, offset, SEEK_SET);
	if (fwrite(pageData, usedLength, 1, fp) == 0) {
		fclose(fp);
		return false;
	}	
	else {
		fclose(fp);
		dirty = false;
		return true;
	}
}

bool page::deleteFromPage(int offset) {
	if (usedLength <rowSize)
		return false;
	//memmove(pageData+offset,pageData+offset+rowSize,usedLength-offset);
	//usedLength -= rowSize;
	memset(pageData + offset, 0, rowSize);
	dirty = true;
	memset(pageData + offset, 1, 1);
	return true;
}

bool page::insertIntoPage(char* newData) {
	if (usedLength + rowSize > PAGE_SIZE)
		return false;
	int offset = pageId*PAGE_SIZE;
	memcpy(pageData + usedLength, newData, rowSize);
	dirty = true;
	usedLength += rowSize;
	return true;
}

//==========memoryPool=====================
memoryPool::memoryPool(string tName) {
	tableName = tName;
	dirty = false;
	isFull = false;
	usedPage = 0;
	strcpy(fileName, TABLEPATH);
	strcat(fileName, tableName.c_str());
	strcat(fileName, ".sdat");
}

bool memoryPool::getTableInfo() {
	FILE* fp= fopen( fileName, "rb+");
	
	pagePool[0] = new page(tableName);
	pagePool[0]->usedLength = PAGE_SIZE*fread(pagePool[0]->pageData, PAGE_SIZE, 1, fp);
	fclose(fp);
	if (pagePool[0]->usedLength ==0)
		return false;

	char* tmp = pagePool[0]->pageData;
	memcpy(&tInfo.tableName, tmp, sizeof(tInfo.tableName));
	tmp += sizeof(tInfo.tableName);
	memcpy(&rowSize, tmp, sizeof(int));
	tmp += sizeof(int);
	memcpy(&tInfo.colNum, tmp, sizeof(int));
	tmp += sizeof(int);

	for (int i = 0; i <tInfo.colNum; i++)
	{
		class data dtmp;
		memcpy(&dtmp, tmp, sizeof(class data));
		tInfo.dataInfos.push_back(dtmp);
		tmp += sizeof(class data);
	}
	pagePool[0]->pageId = 0;
	pagePool[0]->dirty = false;
	pagePool[0]->rowSize = rowSize;
	return true;
}

bool memoryPool::loadBuffer() {
	FILE* fp = fopen( fileName, "rb+");

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, PAGE_SIZE, SEEK_SET);
	fclose(fp);
	usedPage = ceil((double)fsize / PAGE_SIZE);

	for (int i = 1; i < usedPage; i++)
	{
		pagePool[i] = new page(tableName, rowSize);
		pagePool[i]->pageId = i;
		pagePool[i]->loadPage();
	}

	return true;
}

void memoryPool::dropBuffer() {
	for (int i = 0; i < usedPage; i++)
	{
		delete pagePool[i];
	}
}

bool memoryPool::writeBackBuffer() {
	for (int i = 1; i < usedPage; i++)
	{
		if (pagePool[i]->dirty == true)
			pagePool[i]->writeBackPage();
	}
	return true;
}

bool memoryPool::writeBackBuffer(int pageId) {
	return pagePool[pageId]->writeBackPage();
}

bool memoryPool::insertIntoBuffer(char* newData) {
	if ((pagePool[usedPage-1]->usedLength +rowSize)> PAGE_SIZE)
	{
		pagePool[usedPage ] = new page(tableName, rowSize);
		pagePool[usedPage]->pageId = usedPage;
		usedPage++;
		return pagePool[usedPage-1]->insertIntoPage(newData);
	}
	else {
		return pagePool[usedPage-1]->insertIntoPage(newData);
	}
}

bool memoryPool::insertIntoBuffer(vector<class element>elements) {
	char *newData;
	newData = (char*)malloc(sizeof(char)*rowSize);
	memset(newData, 0, rowSize);
	for (int i = 0; i < tInfo.colNum; i++) {
		memcpy(newData+i* sizeof(element), &elements[i], sizeof(element));
	}
	return insertIntoBuffer(newData);
}

bool memoryPool::deleteFromBuffer(int offset) {
	int thisPage = offset / PAGE_SIZE;
	int pageInnerOffset = offset - PAGE_SIZE*thisPage;
	return pagePool[thisPage]->deleteFromPage(pageInnerOffset);
}

void memoryPool::seekLazyDelete() {

}

bool memoryPool::deleteTable() {
	if (remove(fileName) == 0)
		return true;
	else
		return false;
}

//==========bufferManager=====================
void bufferManager::createTable(string tableName, Record record) {
	char fileName[64];
	memset(fileName, 0, 64);
	strcpy(fileName, TABLEPATH);
	strcat(fileName, tableName.c_str());
	strcat(fileName, ".sdat");

	FILE* fp = fopen(fileName, "wb+");
	if (fp == NULL)
		printf("ERROR CREATING FILE\n");
	char writeBuff[4096];
	memset(writeBuff, 0, 4096);
	char* tmp = writeBuff;
	memmove(tmp,record.tInfo.tableName, sizeof(record.tInfo.tableName));
	tmp += sizeof(record.tInfo.tableName);
	memmove(tmp, &record.rowSize, sizeof(int));
	tmp += sizeof(int);
	memmove(tmp, &record.tInfo.colNum, sizeof(int));
	tmp += sizeof(int);
	for (int i = 0; i < record.tInfo.dataInfos.size(); i++)
	{
		memmove(tmp, &record.tInfo.dataInfos[i], sizeof(record.tInfo.dataInfos[i]));
		tmp = tmp + sizeof(record.tInfo.dataInfos[i]);
	}

	fwrite(writeBuff, 4096, 1, fp);
	fclose(fp);
}

void bufferManager::newTableBuffer(string tableName) {
	memoryPool* mp = new memoryPool(tableName);
	tableBufferMap[tableName]=mp;
}

void bufferManager::dropTableBuffer(string tableName) {
	tableBufferMap[tableName]->dropBuffer();
	delete tableBufferMap[tableName];
	tableBufferMap.erase(tableName);
}

bool bufferManager::getTableInfo(string tableName) {
	return tableBufferMap[tableName]->getTableInfo();
}

bool bufferManager::loadTable(string tableName) {
	return tableBufferMap[tableName]->loadBuffer();
}

bool bufferManager::writeBackTable(string tableName) {
	return tableBufferMap[tableName]->writeBackBuffer();
}

char* bufferManager::queryTable(string tableName) {
	if (tableBufferMap[tableName]->usedPage == 1)
		return NULL;
	else
		return (char*)tableBufferMap[tableName]->pagePool[1]->pageData;
}

char* bufferManager::queryTable(string tableName,int offset) {
	int thisPage = offset / PAGE_SIZE;
	int pageInnerOffset = offset - PAGE_SIZE*thisPage;
	if (tableBufferMap[tableName]->usedPage - 1 < thisPage)
		return NULL;
	else
		return (char*)tableBufferMap[tableName]->pagePool[thisPage]->pageData + pageInnerOffset;
}

bool bufferManager::insertIntoTable(string tableName,char* newData) {
	return tableBufferMap[tableName]->insertIntoBuffer(newData);
}

bool bufferManager::insertIntoTable(string tableName, vector<class element>elements) {
	return tableBufferMap[tableName]->insertIntoBuffer(elements);
}

bool bufferManager::deleteFromTable(string tableName,int offset) {
	return tableBufferMap[tableName]->deleteFromBuffer(offset);
}

bool bufferManager::deleteTable(string tableName) {
	if (tableBufferMap[tableName]->deleteTable())
	{
		tableBufferMap.erase(tableName);
		return true;
	}
	else
		return false;
}