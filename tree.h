#ifndef _TREE_H
#define _TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define CHILDREN_NUMBER 4
#define RESERVED_NUMBER 8

typedef enum{
    NODE_DROP,NODE_CREATE,NODE_INSERT,NODE_SELECT,NODE_UPDATE, NODE_END,NODE_CINDEX,NODE_DINDEX,
    NODE_DELETE,NODE_VALUE,NODE_TYPE,NODE_TYPE_LIMIT,NODE_OP,NODE_NUM, NODE_FNUM, NODE_STRING,
    NODE_PRI
}NodeKind;

typedef struct treeNode{
    struct treeNode* child[CHILDREN_NUMBER];
    struct treeNode* sibling;
    NodeKind nodeKind;
    int lineno;

    union{
        int op;
        char* name;
        int val;
        char char_val;
        char* string_val;
        float real_val;
    }attr;

}TreeNode;

TreeNode* newNode(int type);

void freeNode(TreeNode*);

TreeNode* IDWrap(const char*);
TreeNode* NUMWrap(int s);
TreeNode* FNUMWrap(float s);

TreeNode* LinkNode(TreeNode*, TreeNode*);

char* copyString(const char*);

#endif