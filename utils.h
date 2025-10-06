#ifndef UTILS_h
#define UTILS_h
#include"routing.h"

void addOperator(OperatorNode **root, int id);
void removeOperator(OperatorNode **root, int id);
void operatorStatus(OperatorNode *root);
OperatorNode* linearSearch(OperatorNode *root, int target);
#endif
