%{
	#include "global.h"
	#include "tree.h"
	#include "read_write.h"
	#include "lex.yy.c"

	static TreeNode* parserTree;
	FILE* listing;
	int traceScan = true;

	void yyerror(const char* s);

%}

%token CREATE UPDATE DELETE INSERT SELECT DROP TABLE FROM INTO VALUES WHERE SET
%token INT DOUBLE VARCHAR DATE PRIMARY KEY COMMA STAR LP RP EQUAL SEMI ON
%token NOT MYNULL UNIQUE AND INDEX END
%token UNEQUAL GE GT LE LT

%token <name> STRING
%token <name> ID 
%token <num> INTEGER_VALUE
%token <fnum> REAL_VALUE

%start line_list

%union {
	int num;
	float fnum;
	char ch;
	char* name;
	TreeNode* node;
	//std::string _string;
}

%type <node> expression value_name_list value_name type type_limit column_list
%type <node> value_list where_limit where_single equal_list equal_single value
%type <num> op

%%

line_list :
 	line_list line |
	line
	;

line :
	expression SEMI {
		if($1->nodeKind == NODE_CREATE){
			createFromTree($1);
		}
		else if($1->nodeKind == NODE_DROP){
			//printf("start drop");
			dropFromTree($1);
		}
		else if($1->nodeKind == NODE_INSERT){
			insertFromTree($1);
		}
		else if($1->nodeKind == NODE_SELECT){
			selectFromTree($1);
		}
		else if($1->nodeKind == NODE_UPDATE){
			updateFromTree($1);
		}
		else if($1->nodeKind == NODE_DELETE){
			deleteFromTree($1);
		}
		else if($1->nodeKind == NODE_END){
			writeBack();
		}
		else if($1->nodeKind == NODE_CINDEX){
			createIndex($1);
		}
		else if($1->nodeKind == NODE_DINDEX){
			dropIndex($1);
		}
		else{
			printf("Aha! A TREE_TYPE ERROR!\n");
		}
		freeNode($1);
	};

expression :
	CREATE TABLE ID LP value_name_list RP {
		$$ = newNode(1);
		$$->attr.name = copyString($3);
		$$->child[0] = $5;
	}
	| DROP TABLE ID {
		$$ = newNode(0);
		$$->attr.name = copyString($3);
	}
	| INSERT INTO ID LP column_list RP VALUES LP value_list RP {

		$$ = newNode(2);
		$$->attr.name = copyString($3);
		$$->child[0] = $5;
		$$->child[1] = $9;
	}
	| INSERT INTO ID VALUES LP value_list RP {

		$$ = newNode(2);
		$$->attr.name = copyString($3);
		$$->child[0] = NULL;
		$$->child[1] = $6;
	}
	| SELECT STAR FROM ID {

		$$ = newNode(3);
		$$->attr.name = copyString($4);
		$$->child[0] = NULL;
		$$->child[1] = NULL;
	}
	| SELECT STAR FROM ID WHERE where_limit {

		$$ = newNode(3);
		$$->attr.name = copyString($4);
		$$->child[0] = NULL;
		$$->child[1] = $6;
	}
	| SELECT column_list FROM ID{

		$$ = newNode(3);
		$$->attr.name = copyString($4);
		$$->child[0] = $2;
		$$->child[1] = NULL;
	}
	| SELECT column_list FROM ID WHERE where_limit {

		$$ = newNode(3);
		$$->attr.name = copyString($4);
		$$->child[0] = $2;
		$$->child[1] = $6;
	}
	| UPDATE ID SET equal_list {

		$$ = newNode(4);
		$$->attr.name = copyString($2);
		$$->child[0] = $4;
		$$->child[1] = NULL;
	}
	| UPDATE ID SET equal_list WHERE where_limit {

		$$ = newNode(4);
		$$->attr.name = copyString($2);
		$$->child[0] = $4;
		$$->child[1] = $6;
	}
	| DELETE FROM ID {

		$$ = newNode(5);
		$$->attr.name = copyString($3);
		$$->child[0] = NULL;
	}
	| DELETE FROM ID WHERE where_limit {

		$$ = newNode(5);
		$$->attr.name = copyString($3);
		$$->child[0] = $5;
	}
	| END {
		$$ = newNode(100);
	}
	| CREATE INDEX ID ON ID LP ID RP{
		$$ = newNode(101);
		$$->attr.name = copyString($5);
		$$->child[0] = IDWrap($3);
        $$->child[1] = IDWrap($7);
	}
	| DROP INDEX ID ON ID LP ID RP{
		$$ = newNode(102);
		$$->attr.name = copyString($5);
		$$->child[0] = IDWrap($3);
        $$->child[1] = IDWrap($7);
	}
	| {
		printf("something test?\n");
	};

value_name_list :
	value_name_list COMMA value_name {
		$$ = LinkNode($1,$3);
	}
	| value_name {
		$$ = $1;
	};

value_name :
	ID type type_limit {
		$$ = newNode(6);
		$$->attr.name = copyString($1);
		$$->child[0] = $2;
		$$->child[1] = $3;
	}
	| PRIMARY KEY LP ID RP{
		$$ = newNode(104);
		$$->attr.name = copyString($4);
	};

type :
	INT {
		$$ = newNode(7);
		$$->attr.op = 1;
	}
	| VARCHAR LP INTEGER_VALUE RP {
		$$ = newNode(7);
		$$->attr.op = 3;
		$$->child[0] = NUMWrap($3);
	}
	| DOUBLE {
		$$ = newNode(7);
		$$->attr.op = 2;
	};

type_limit :
	NOT MYNULL {
		$$ = newNode(8);
		$$->attr.op = 4;
	} 
	| UNIQUE {
		$$ = newNode(8);
		$$->attr.op = 5;
	} 
	| {
		$$ = NULL;
	};


column_list :
	column_list COMMA ID {
		$$ = LinkNode($1,IDWrap($3));
	}
	| ID {
		$$ = IDWrap($1);
	};

value_list :
	value_list COMMA value {
		$$ = LinkNode($1,$3);
	}	
	| value {
		$$ = $1;
	};

value :
	STRING {
		$$ = IDWrap($1);
	}
	| INTEGER_VALUE {
		$$ = NUMWrap($1);
	}
	| REAL_VALUE {
		$$ = FNUMWrap($1);
	}

where_limit :
	where_limit COMMA where_single {
		$$ = LinkNode($1,$3);
	}
	| where_limit AND where_single {
		$$ = LinkNode($1,$3);
	}
	| where_single {
		$$ = $1;
	};

where_single :
	ID op value {
		$$ = newNode(9);
		$$->attr.op = $2;
		$$->child[0] = IDWrap($1);
		$$->child[1] = $3;
	};

op :
	EQUAL {
		$$ = 1;
	}
	| UNEQUAL {
		$$ = 2;
	}
	| GE {
		$$ = 3;
	}
	| GT {
		$$ = 4;
	}
	| LE {
		$$ = 5;
	}
	| LT {
		$$ = 6;
	};

equal_list :
	equal_list COMMA equal_single {
		$$ = LinkNode($1,$3);
	}
	| equal_single {
		$$ = $1;
	};

equal_single :
	ID EQUAL value {
		$$ = newNode(9);
		$$->attr.op = 1;
		$$->child[0] = IDWrap($1);
		$$->child[1] = $3;
	};

%%

int main(){
	yyparse();
	return 0;
}

void yyerror(const char* s){
	printf("Error at line %d: %s\n", lineno, s);
}
