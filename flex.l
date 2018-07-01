%{

#include "global.h"



int lineno = 1;

extern FILE* listing;

static void

update_lineno (const char *l, size_t len){

	while (len-- > 0)

		if (*l++ == '\n')

			lineno++;

}

%}

%%

"create" 	{ return CREATE;}
"CREATE"	{ return CREATE;}

"update"    { return UPDATE;}
"UPDATE"	{ return UPDATE;}

"delete"  	{ return DELETE;}
"DELETE"	{ return DELETE;}

"insert"  	{ return INSERT;}
"INSERT"	{ return INSERT;}

"select"  	{ return SELECT;}
"SELECT"	{ return SELECT;}

"drop"	  	{ return DROP;}
"DROP"		{ return DROP;}

"table"   	{ return TABLE;}
"TABLE"		{ return TABLE;}

"from"    	{ return FROM;}
"FROM"		{ return FROM;}

"into"		{ return INTO;}
"INTO"		{ return INTO;}

"values"	{ return VALUES;}
"VALUES"	{ return VALUES;}

"where"		{ return WHERE;}
"WHERE"		{ return WHERE;}

"set"		{ return SET;}
"SET"		{ return SET;}

"double"	{ return DOUBLE;}
"DOUBLE"	{ return DOUBLE;}

"float"		{ return DOUBLE;}
"FLOAT"		{ return DOUBLE;}

"int"		{ return INT;}
"INT"		{ return INT;}

"varchar"	{ return VARCHAR;}
"VARCHAR"	{ return VARCHAR;}

"date"		{ return DATE;}
"DATE"		{ return DATE;}

"primary"	{ return PRIMARY;}
"PRIMARY"	{ return PRIMARY;}

"key"		{ return KEY;}
"KEY"		{ return KEY;}

"not"		{ return NOT;}
"NOT"		{ return NOT;}

"null"		{ return MYNULL;}
"NULL"		{ return MYNULL;}

"unique"    { return UNIQUE;}
"UNIQUE"	{ return UNIQUE;}

"index"		{ return INDEX;}
"INDEX"		{ return INDEX;}

","        	{ return COMMA;}
"*"			{ return STAR;}
"("        	{ return LP;}
")"         { return RP;}
"="         { return EQUAL;}
";"         { return SEMI;}
"<>"		{ return UNEQUAL;}
"!="  		{ return UNEQUAL;}
">="        { return GE;}
">"        	{ return GT;}
"<="        { return LE;}
"<"         { return LT;}

"and"		{ return AND;}
"AND"		{ return AND;}

"END"		{ return END;}
"end"		{ return END;}

"on"		{ return ON;}
"ON"		{ return ON;}
[0-9]+				    {yylval.num = atoi(yytext); return INTEGER_VALUE;}

[0-9]+.[0-9]+		    {yylval.fnum = atof(yytext); return REAL_VALUE;}

\'[^\']*\' 					{yylval.name = strdup(yytext); return STRING;}
\"[^\"]*\" 					{yylval.name = strdup(yytext); return STRING;}

[[:alpha:]_][[:alnum:]_]* 	{yylval.name = strdup(yytext); return ID;}

[[:space:]]+ 			{ update_lineno (yytext, yyleng);}

.						{ printf("Unexpected character %s at line %d\n", yytext, lineno);}

%%

int yywrap(){

	return 1;

}
