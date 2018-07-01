#include "read_write.h"
#include "BufferManger.h"
#include "IndexManager.h"
#include "Record.h"
#include "element.h"
#include "time.h"

static bufferManager bfm;
//static std::string lasttbname = NULL;
static std::string lasttbname = "";
//ok
void createFromTree(TreeNode* tree) {
	clock_t start, end;
	//actually, changedLine of CREATE is always 0;
	int changedLine = 0;
	/*
	tableName: tree->attr.name
	tableAttribute: tree->child[0]
		这个节点以及兄节点记录arrribute属性
		int columnNum = 0;
		TreeNode* temp = tree->child[0];
		while(temp){
			temp->attr.name    属性名
			temp->child[0]->attr.op     type类型 INT: 1 VARCHAR: 3 DOUBLE: 2
										如果是VARCHAR 则temp->child[0]->child[0]->attr.val即为varchar长度
			temp->child[1]     typeLimit 如果temp->child[1]->attr.op = 4，则是not null，5是unique。

			//这里写存入代码

			columnNum++;
			temp = temp->sibling
		}

	 */

	 //!primary key还没写
	start = clock();
	std::string tbname = tree->attr.name;

	std::vector<class data> column;
	bool ifunique;
	TreeNode* temp = tree->child[0];
	while (temp) {
		if(temp->nodeKind == NODE_PRI){
			for(auto ii = column.begin(); ii != column.end(); ii++){
				if(0 == strcmp(ii->dataName, temp->attr.name)){
					ii->isPrimaryKey = true;
					break;
				}
			}
			temp = temp->sibling;
			continue;
		}
		if (temp->child[1] != NULL && temp->child[1]->attr.op == 5)
			ifunique = true;
		else
			ifunique = false;
		class data wtftemp = data(temp->child[0]->attr.op, temp->attr.name, ifunique, false);
		column.push_back(wtftemp);

		temp = temp->sibling;
	}

	//bfm.createTable(tree->attr.name, Record(tableInfo()));
	bfm.createTable(tbname, Record(tableInfo(tree->attr.name, column)));
	end = clock();

	bfm.newTableBuffer(tbname);
	if ( bfm.getTableInfo(tbname)== false)
		cout << "fail to load table info" << endl;
	bfm.loadTable(tbname);
	printf("Query OK, 0 rows affected(%.2lf sec)\n", (end - start) / 1000.0);

}
//ok
void dropFromTree(TreeNode* tree) {
	if(tree == NULL){
		printf("Are you sure to drop?? Aha!");
	}
	//actually, changedLine of DROP is always 0;
	int changedLine = 0;

	/*
	tableName: tree->attr.name

	//这里写drop代码
	 */
	clock_t start, end;
	start = clock();
	
	std::string tbname = tree->attr.name;

	if(ifexistInBuffer(tbname)){
		bfm.getTableInfo(tbname);
	}
	else{
		bfm.newTableBuffer(tbname);
		if (bfm.getTableInfo(tbname) == false){
			printf("getTableInfo error\n");
		}
		bfm.loadTable(tbname);
	}

	//cout << "look! "<< tbname;
	//bfm.dropTableBuffer(tbname);
	bfm.deleteTable(tbname);
	end = clock();
	printf("Query OK, 0 rows affected(%.2lf sec)\n", (end - start) / 1000.0);
}

void selectFromTree(TreeNode* tree) {
	//the number of select result
	int changedLine = 0;

	/*
	tableName: tree->attr.name

	if(tree->child[0] == NULL)
		//这里说明是select*
	else
		//select有属性 属性是 tree->child[0]
		TreeNode* temp = tree->child[0];
		while(temp){
			temp->attr.name 读取的属性名
			changedLine++;
			temp = temp->sibling;
		}

	tree->child[1]为where_limit
	!!!here这里晚点写
	 */
	clock_t start, end;
	int rowSize;
	int column;
	std::string tbname = tree->attr.name;


	start = clock();

	if(ifexistInBuffer(tbname)){
		bfm.getTableInfo(tbname);
	}
	else{
		bfm.newTableBuffer(tbname);
		if (bfm.getTableInfo(tbname) == false){
			printf("getTableInfo error\n");
		}
		bfm.loadTable(tbname);
	}
	char* a = bfm.queryTable(tbname);
	char* cnm = a;

	rowSize = bfm.tableBufferMap[tbname]->rowSize;
	//列消息
	column = bfm.tableBufferMap[tbname]->tInfo.colNum;


	int* shitcolumn = (int*)malloc((column + 1)*sizeof(char) );

	TreeNode* shittemp = tree->child[0];
	if(shittemp == NULL){
		for(int i = 0; i < column; i++)
			shitcolumn[i] = i;
		shitcolumn[column] = -1;
	}
	else{
		int shit = 0;
		while(shittemp){
			int i;
			for(i = 0; i < column; i++){
				if( 0 == strcmp(bfm.tableBufferMap[tbname]->tInfo.dataInfos[i].dataName, shittemp->attr.name))
					break;
			}
			if(i < column){
				shitcolumn[shit] = i;
				shit++;
			}
			shittemp = shittemp->sibling;
		}
		shitcolumn[shit] = -1;
	}

	//打印表头
	//

	printf("___________________________\n");
	for (int i = 0; shitcolumn[i] != -1; i++) {
		printf("| %s ", (bfm.tableBufferMap[tbname]->tInfo.dataInfos[shitcolumn[i]].dataName));

	}
	printf(" |\n");
	printf("___________________________\n");
	
	/*
	element* temp_630 = new element;
	memcpy(temp_630, a, sizeof(element));
	temp_630->test();
	*/
	/*
	printf("___________________________\n");
	for (int i = 0; i < column; i++) {
		printf("| %s ", (bfm.tableBufferMap[tbname]->tInfo.dataInfos[i].dataName));

	}
	printf(" |\n");
	printf("___________________________\n");
	*/
	//打印数据
	//a = a + 4096;
	int k;
	int tempUsesize;
	for (int j = 1;; j++) {
		//遍历每一页
		a = bfm.queryTable(tbname, j * 4096);
		tempUsesize = bfm.tableBufferMap[tbname]->pagePool[j]->usedLength;
		if(tempUsesize == 0)
			goto myempty;
		//printf("tempUsesize %d", tempUsesize);
		vector<element*> myele;
		for (k = 0; k < tempUsesize; k += rowSize) {
			//把所有元素读进myele
			char* tempa = a;
			for (int i = 0; i < column; i++) {
				element* temp_630 = new element;
				memcpy(temp_630, a, sizeof(element));
				myele.push_back(temp_630);
				//myele.push_back(reinterpret_cast<element*>(a));
				//myele[0]->test();
				//a += bfm.tableBufferMap[tbname]->rowSize;
				a += sizeof(element);
				//printf("offset: %d\n", a - cnm);
			}
			a = tempa + rowSize;

			//如果是false，则清空myele并开始下一行
			if (judgeRow(tree->child[1], column, bfm.tableBufferMap[tbname]->tInfo.dataInfos, myele)
				== false) {
				myele.clear();
				continue;
			}
			//遍历这一行的元素
			
			//myele[0]->test();
			//myele[1]->test();

			for (int i = 0; shitcolumn[i] != -1; i++) {
				printf("| ");
				if (myele[shitcolumn[i]]->dType == 1) {
					printf("%d", myele[shitcolumn[i]]->innerElement.intData);
				}
				else if (myele[shitcolumn[i]]->dType == 2) {
					printf("%f", myele[shitcolumn[i]]->innerElement.floatData);
				}
				else if (myele[shitcolumn[i]]->dType == 3) {
					printf("%s", myele[shitcolumn[i]]->innerElement.varchar);
				}
				else{
					printf("Something strange type?\n");
				}
			}
			printf(" |\n");
		
		//printf("!!!!!!!!!\n");
		/*
		for (int i = 0; i < column; i++) {
				printf("| ");
				if (myele[i]->dType == 1) {
					printf("%d", myele[i]->innerElement.intData);
					printf(" |\n");
				}
				else if (myele[i]->dType == 2) {
					printf("%f", myele[i]->innerElement.floatData);
					printf(" |\n");
				}
				else if (myele[i]->dType == 3) {
					printf("%s", myele[i]->innerElement.varchar);
					printf(" |\n");
				}
				else{
					//printf("cn %d\n", myele[i]->dType);
					//printf("Something strange type?\n");
				}
			}
		*/
			
			printf("___________________________\n");
			changedLine++;
			myele.clear();

		}

		if (k == 4096)
			continue;
		else
			break;
	}
myempty:
	end = clock();
	printf("Query OK, %d rows affected(%.2lf sec)\n", changedLine, (end - start) / 1000.0);

}

void deleteFromTree(TreeNode* tree) {
		//the number of select result
	int changedLine = 0;

	clock_t start, end;
	int rowSize;
	int column;
	std::string tbname = tree->attr.name;

	if(ifexistInBuffer(tbname)){
		bfm.getTableInfo(tbname);
	}
	else{
		bfm.newTableBuffer(tbname);
		if (bfm.getTableInfo(tbname) == false){
			printf("getTableInfo error\n");
		}
		bfm.loadTable(tbname);
	}
	char* a = bfm.queryTable(tbname);

	start = clock();
	if (bfm.getTableInfo(tbname) == false)
		bfm.loadTable(tbname);

	rowSize = bfm.tableBufferMap[tbname]->rowSize;
	//列消息
	column = bfm.tableBufferMap[tbname]->tInfo.colNum;

	/*
	element* temp_630 = new element;
	memcpy(temp_630, a, sizeof(element));
	temp_630->test();
	*/

	//打印数据
	//a = a + 4096;
	int k;
	int tempUsesize;
	int deleterow = -1;
	for (int j = 1;; j++) {
		deleterow = -1;
		//遍历每一页
		a = bfm.queryTable(tbname, j * 4096);
		tempUsesize = bfm.tableBufferMap[tbname]->pagePool[j]->usedLength;
		if(tempUsesize == 0)
			goto myempty;
		//printf("tempUsesize %d", tempUsesize);
		vector<element*> myele;
		for (k = 0; k < tempUsesize; k += rowSize) {
			//把所有元素读进myele
			char* tempa = a;
			for (int i = 0; i < column; i++) {
				element* temp_630 = new element;
				memcpy(temp_630, a, sizeof(element));
				myele.push_back(temp_630);

				a += sizeof(element);
			}
			deleterow++;
			a = tempa + rowSize;

			if (judgeRow(tree->child[0], column, bfm.tableBufferMap[tbname]->tInfo.dataInfos, myele)
				== false) {
				myele.clear();
				continue;
			}
			//遍历这一行的元素
			
			//myele[0]->test();
			//myele[1]->test();
			//deleterow = 0;
			//printf("%d\n",deleterow * rowSize);
			bfm.deleteFromTable(tbname, 4096 * j + deleterow * rowSize);
			/*if(lasttbname == tbname){
				if(!lasttbname.empty())
					bfm.writeBackTable(lasttbname);
				lasttbname = tbname;
			}*/
			bfm.writeBackTable(tbname);
			changedLine++;
			myele.clear();

		}

		if (k == 4096)
			continue;
		else
			break;
	}
myempty:
	end = clock();
	printf("Query OK, %d rows affected(%.2lf sec)\n", changedLine, (end - start) / 1000.0);


}
//ok
void insertFromTree(TreeNode* tree) {
	//the number of insert result
	int changedLine = 0;
	clock_t start, end;
	/*
	tableName: tree->attr.name
	tree->child[0]: column_list, NULL则为all
	tree->child[1]: value_list
	 */


	std::string tbname = tree->attr.name;

	if(ifexistInBuffer(tbname)){
		bfm.getTableInfo(tbname);
	}
	else{
		bfm.newTableBuffer(tbname);
		if (bfm.getTableInfo(tbname) == false){
			printf("getTableInfo error\n");
		}
		bfm.loadTable(tbname);
	}

	std::vector<element> myele;
	TreeNode* temp = tree->child[1];
	start = clock();
	while (temp) {
		if (temp->nodeKind == NODE_NUM) {
			myele.push_back(element((int)temp->attr.val));
		}
		else if (temp->nodeKind == NODE_FNUM) {
			myele.push_back(element(float(temp->attr.real_val)));
		}
		else if (temp->nodeKind == NODE_STRING) {
			//myele.push_back(element("ddd", sizeof("ddd")));
			myele.push_back(element(temp->attr.name, strlen(temp->attr.name)));
		}
		temp = temp->sibling;
	}
		//printf("insert:\n");
		//myele[0].test();
		//myele[1].test();
	bfm.insertIntoTable(tbname, myele);
	/*if(lasttbname == tbname){
		if(!lasttbname.empty())
			bfm.writeBackTable(lasttbname);
		lasttbname = tbname;
	}*/
	//bfm.writeBackTable(tbname);
	end = clock();
	printf("Query OK, 1 rows affected(%.2lf sec)\n", (end - start) / 1000.0);
}



//do nothing
void updateFromTree(TreeNode* tree) {
	int changedLine = 0;

	//这个不需要写
		/*
	tableName: tree->attr.name
	tree->child[1]: where_list, NULL则为all
	tree->child[0]: equal_list
	 */
}

bool ifture(int left, int op, int value) {

	if (op == 1) {
		return left == value;
	}
	else if (op == 2) {
		return left != value;
	}
	else if (op == 3) {
		return left >= value;
	}
	else if (op == 4) {
		return left > value;
	}
	else if (op == 5) {
		return left <= value;
	}
	else if (op == 6) {
		return left < value;
	}
	return false;
}
bool iftrue(float left, int op, float value) {
	if (op == 1) {
		return left == value;
	}
	else if (op == 2) {
		return left != value;
	}
	else if (op == 3) {
		return left >= value;
	}
	else if (op == 4) {
		return left > value;
	}
	else if (op == 5) {
		return left <= value;
	}
	else if (op == 6) {
		return left < value;
	}
	return false;
}
bool iftrue(char* left, int op, char* value) {
	if (op == 1) {
		return strcmp(left, value) == 0;
	}
	else if (op == 2) {
		return strcmp(left, value) != 0;
	}
	else if (op == 3) {
		return strcmp(left, value) >= 0;
	}
	else if (op == 4) {
		return strcmp(left, value) > 0;
	}
	else if (op == 5) {
		return strcmp(left, value) <= 0;
	}
	else if (op == 6) {
		return strcmp(left, value) < 0;
	}
	return false;
}

bool judgeRow(TreeNode* whereList, int column_num, vector <class data> column_name, vector<element*> ele) {
	if(ele[0]->lazyDelete == true)
		return false;
	if (whereList == NULL)
		return true;
	TreeNode* temp = whereList;
	while (temp) {
		//对于每个限制，都用每个数据便利一次
		for (int i = 0; i < column_num; i++) {
			if (strcmp(column_name[i].dataName, temp->child[0]->attr.name) == 0) {
				if (ele[i]->dType == 1) {
					if (iftrue((int)ele[i]->innerElement.intData, temp->attr.op, (int)temp->child[1]->attr.val) == false)
						return false;
				}
				else if (ele[i]->dType == 2) {
					if (iftrue((float)ele[i]->innerElement.floatData, temp->attr.op, (float)temp->child[1]->attr.real_val) == false)
						return false;
				}
				else if (ele[i]->dType == 3) {

					//printf("The left is %s\n", ele[i]->innerElement.varchar);
					//printf("The right is %s\n\n",temp->child[1]->attr.string_val);
					if (iftrue(ele[i]->innerElement.varchar, temp->attr.op, temp->child[1]->attr.string_val) == false)
						return false;
				}
			}
		}
		temp = temp->sibling;
	}
	return true;
}

void writeBack(){
	for(auto i = bfm.tableBufferMap.begin(); i != bfm.tableBufferMap.end(); i++){
		bfm.writeBackTable(i->first);
	}
}

bool ifexistInBuffer(std::string tbname){
	for(auto i = bfm.tableBufferMap.begin(); i != bfm.tableBufferMap.end(); i++){
		//std::string stmp = i->first;
		if(strcmp(tbname.c_str(), i->first.c_str()) == 0)
			return true;
	}
	return false;
}

void createIndex(TreeNode* tree){
    indexManager idx;
    std::string tbname = tree->attr.name;
    std::string columnName = tree->child[1]->attr.name;
   // std::string tbname = "he";
   // std::string columnName = "wor";
    idx.createFromRecord(tbname, columnName);
    idx.writeBackIndex(tbname, columnName);
    printf("OK!\n");
	/*
	char*的表名：tree->attr.name
	std::string tbname = tree->attr.name
	char*的索引名： tree->child[0]->attr.name


	select函数，第89到133行得到rowSize和column

	bfm.tableBufferMap[tbname]->tInfo是表头信息，详情见buffermananger.h里的定义

	176到233行，每次得到一行ele的集合，把那些.earse()的语句删了，再把这些ele并起来就是整个表所有的ele，改不来的话先留着
	 */
}
void dropIndex(TreeNode* tree){
	indexManager idx;
	//std::string tbname = "he";
    //std::string columnName = "wor";
 	std::string tbname = tree->attr.name;
    std::string columnName = tree->child[1]->attr.name;
    
    idx.deleteIndex(tbname, columnName);
    //idx.writeBackTable(tbname, columnName);
    printf("OK!\n");

}
