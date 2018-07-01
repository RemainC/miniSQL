#include "IndexManager.h"

bool indexManager::createFromRecord(string tablename, string elename) {
	string tableFileName = TABLEPATH + tablename + ".sdat";
	string idxName = tablename + "_" + elename;

	bfm.newTableBuffer(tablename);
	bfm.getTableInfo(tablename);
	int rs = bfm.tableBufferMap[tablename]->rowSize;
	int cn = bfm.tableBufferMap[tablename]->tInfo.colNum;
	int i;
	for (i = 0; i < bfm.tableBufferMap[tablename]->tInfo.dataInfos.size(); i++) {
		if (bfm.tableBufferMap[tablename]->tInfo.dataInfos[i].dataName == elename)
			break;
	}
	if (i == bfm.tableBufferMap[tablename]->tInfo.dataInfos.size())
		return false;

	FILE* fp = fopen(tableFileName.c_str(), "rb+");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fclose(fp);

	fp = fopen(tableFileName.c_str(), "rb+");
	if (fp == NULL)
		printf("ERROR READING FILE\n");
	long tmpoffset = 0;
	char elementbuff[80];

	fseek(fp, PAGE_SIZE + i * sizeof(element), SEEK_SET);
	vector<bpTreeNodepair> pairvector;
	
	while (1) {
		tmpoffset = ftell(fp);
		fread(elementbuff, 1, sizeof(element), fp);
		element *e = new element();
		memcpy(e, elementbuff, sizeof(element));

		bpTreeNodepair* tmppair=(struct bptnp*)malloc(sizeof(struct bptnp));
		tmppair->e = *e;
		tmppair->offset = tmpoffset;
		pairvector.push_back(*tmppair);

		if (tmpoffset + rs-sizeof(element) >= fsize)
			break;
		fseek(fp, rs - sizeof(element), SEEK_CUR);
		
	}
	indexVector = pairvector;
	fclose(fp);
	return true;

}

bool indexManager::writeBackIndex(string tablename, string elename) {
	string idxFileName = INDEXPATH + tablename + "_" + elename;
	string idxName = tablename + "_" + elename;

	FILE* fp = fopen(idxFileName.c_str(), "wb+");
	char writeBuf[128];
	for (int i = 0; i < indexVector.size(); i++) {
		memset(writeBuf, 0, 128);
		memcpy(writeBuf, &indexVector[i], sizeof(struct bptnp));
		fwrite(writeBuf, 1, 128, fp);
	}
	fclose(fp);
	return true;
}

bool indexManager::deleteIndex(string tablename, string elename) {
	//printf("helo");
	string idxFileName = INDEXPATH + tablename + "_" + elename;
	//std::cout << idxFileName;
	if (remove(idxFileName.c_str()) == 0)
		return true;
	else
		return false;
}