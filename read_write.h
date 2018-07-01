#ifndef _READ_WRITE_H
#define _READ_WRITE_H

#include "global.h"


void createFromTree(TreeNode* tree);
void dropFromTree(TreeNode* tree);
void selectFromTree(TreeNode* tree);
void deleteFromTree(TreeNode* tree);
void insertFromTree(TreeNode* tree);
void updateFromTree(TreeNode* tree);

void createIndex(TreeNode* tree);
void dropIndex(TreeNode* tree);

bool ifture(int left, int op, int value);
bool iftrue(float left, int op, float value);
bool iftrue(char* left, int op, char* value);

bool judgeRow(TreeNode* whereList, int column_num, std::vector<class data> column_name, std::vector<element*> ele);

void writeBack();

bool ifexistInBuffer(std::string last);

#endif