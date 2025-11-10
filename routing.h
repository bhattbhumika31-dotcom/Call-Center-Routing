#ifndef ROUTING_H
#define ROUTING_H

#include <windows.h> // Required for TimerThread definition

// --- Data Structure Definition ---
typedef struct OperatorNode {
    int id;
    int status;
    int callTime;
    char name[64];
    struct OperatorNode *left;
    struct OperatorNode *right;
} OperatorNode;

// --- Threading Helper (Defined in routing.c) ---
DWORD WINAPI TimerThread(LPVOID param); 

// --- Operator Management Functions (Moved from your original utils.h) ---
void addOperator(OperatorNode **root, int id, const char *name);
void removeOperator(OperatorNode **root, int id);
void operatorStatus(OperatorNode *root);
OperatorNode* DFS(OperatorNode *root, int target); // Helper function
OperatorNode* find_free_operator(OperatorNode *root); // find a FREE operator (status==0)
void free_tree(OperatorNode *root); // free entire tree

// --- Core Routing Function ---
void assignCall(OperatorNode **root, int time);

#endif // ROUTING_H