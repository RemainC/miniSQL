#ifndef element_h
#define element_h

#include <string>
#include <vector>

using namespace std;

#define dataType int

class element {
public:
	bool lazyDelete = false;
	dataType dType;
	int length;
	union {
		int intData;
		float floatData;
		char varchar[64];
	}innerElement;

	element() {};
	element(int intData) {
		memset(&innerElement, 0, sizeof(innerElement));
		dType = 1;
		length = sizeof(int);
		innerElement.intData = intData;
	};
	element(float floatData) {
		memset(&innerElement, 0, sizeof(innerElement));
		dType = 2;
		length = sizeof(float);
		innerElement.floatData = floatData;
	};
	element(char* charData, int varCharLength) {
		memset(&innerElement, 0, sizeof(innerElement));
		dType = 3;
		length = varCharLength;
		memcpy(innerElement.varchar, charData, varCharLength);
	};

	void test(){
		printf("TYPE: ");
		printf("%d", dType);
		if(dType == 1)
			printf("value is %d", innerElement.intData);
		else if(dType == 2)
			printf("value is %lf", innerElement.floatData);
		else if(dType == 3)
			printf("value is %s", innerElement.varchar);
		printf("\n");
	}
};

class data {
public:
	dataType dType;
	char dataName[32];
	bool unique;
	bool isPrimaryKey;

	data() {};
	data(dataType dType, char* dName, bool unique, bool isPrimaryKey) :
		dType(dType), unique(unique), isPrimaryKey(isPrimaryKey) {
		memset(dataName, 0, sizeof(dataName));
		strcpy(dataName, dName);
	};
};

class tableInfo {
public:
	char tableName[64];
	int colNum;
	vector <class data>dataInfos;

	tableInfo() {};
	tableInfo(char* tName, vector<class data>dataInfos) :
		dataInfos(dataInfos) {
		memset(tableName, 0, sizeof(tableName));
		strcpy(tableName, tName);
		colNum = dataInfos.size();
	};
};

#endif