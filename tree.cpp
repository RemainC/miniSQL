#include "tree.h"
#include <time.h>
extern FILE* listing;
extern int lineno;

TreeNode* newNode(int type){
    TreeNode* t = (TreeNode *)malloc(sizeof(TreeNode));
    if(t == NULL){
        fprintf(listing,"Out of memory error at line %d\n",lineno);
        return NULL;
    }

    int i;
    for(i = 0; i < CHILDREN_NUMBER; i++){
        t->child[i] = NULL;
    }

    t->sibling = NULL;
    t->lineno = lineno;

    if(type == 0){
        t->nodeKind = NODE_DROP;
    }
    else if(type == 1){
        t->nodeKind = NODE_CREATE;
    }
    else if(type == 2){
        t->nodeKind = NODE_INSERT;
    }
    else if(type == 3){
        t->nodeKind = NODE_SELECT;
    }
    else if(type == 4){
        t->nodeKind = NODE_UPDATE;
    }
    else if(type == 5){
        t->nodeKind = NODE_DELETE;
    }
    else if(type == 6){
        t->nodeKind = NODE_VALUE;
        //节点1记录type 节点2记录limit
    }
    else if(type == 7){
        t->nodeKind = NODE_TYPE;
    }
    else if(type == 8){
        t->nodeKind = NODE_TYPE_LIMIT;
    }
    else if(type == 9){
        t->nodeKind = NODE_OP;
    }
    else if(type == 10){
        t->nodeKind = NODE_NUM;
    }
    else if(type == 11){
        t->nodeKind = NODE_FNUM;
    }
    else if(type == 12){
        t->nodeKind = NODE_STRING;
    }
    else if(type == 100){
        t->nodeKind = NODE_END;
    }
    else if(type == 101){
        t->nodeKind = NODE_CINDEX;
    }   
    else if(type == 102){
        t->nodeKind = NODE_DINDEX;
    }
    else if(type == 104){
        t->nodeKind = NODE_PRI;
    }
    return t;
}


void freeNode(TreeNode* node){
    if(node == NULL)
        return;
    int i;
    for(i =0; i < CHILDREN_NUMBER; i++)
        freeNode(node->child[i]);
    freeNode(node->sibling);
    free(node);
}

TreeNode* IDWrap(const char* s){
    TreeNode* x = newNode(12);
    x->attr.name = copyString(s);
    //printf("!!!\n%s\n%d\n!!!", x->attr.name, clock());
    return x;
}

TreeNode* NUMWrap(int s){
    TreeNode* x = newNode(10);
    x->attr.val = s;
    return x;
}

TreeNode* FNUMWrap(float s){
    TreeNode* x = newNode(11);
    x->attr.real_val = s;
    return x;
}

TreeNode* LinkNode(TreeNode* a, TreeNode* b){
    if(a == NULL)
        return b;
    TreeNode* temp = a;
    while(temp->sibling != NULL){
        temp = temp->sibling;
    }
    temp->sibling = b;
    return a;
}

char* copyString(const char* s){
    if(s == NULL)
        return NULL;
    char *t;
    t = (char*)malloc(sizeof(s));
    if(t == NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else
        strcpy(t,s);
    return t;
}

