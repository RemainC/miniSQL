#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>


#include "tree.h"
#include "element.h"

#ifndef testlex
#define testlex
#include "y.tab.h"
#endif

#define true 1
#define false 0
#define ENDFILE 0

extern FILE* listing;
extern int lineno;
extern int TraceScan;


#endif