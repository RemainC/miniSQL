#ifndef Record_h
#define Record_h

#include"element.h"

#define TABLEPATH "/Users/remain/Downloads/new/data/table/"

class Record {
public:
	tableInfo tInfo;
	int rowSize;
	vector<element> elements;

	Record() {};
	Record(tableInfo t) {
		tInfo = t;
		rowSize = 256;
	}
	Record(tableInfo t, vector<element> e) {
		tInfo = t;
		elements = e;

		int s = sizeof(t.dataInfos);
		rowSize = s * t.dataInfos.size();
		if (rowSize < 256)
			rowSize = 256;
		else if (rowSize < 512)
			rowSize = 512;
		else if (rowSize < 1024)
			rowSize = 1024;
		else if (rowSize < 2048)
			rowSize = 2048;
		else if (rowSize < 4096)
			rowSize = 4096;

	};
};

#endif
