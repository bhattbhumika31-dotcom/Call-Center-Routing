#ifndef ROUTING_H
#define ROUTING_H

#include <windows.h> // Required for TimerThread definition

// --- Data Structure Definition ---
typedef struct OperatorNode {
    int id;
    int status;
    int callTime;
    struct OperatorNode *next;
} OperatorNode;

// --- Threading Helper (Defined in routing.c) ---
DWORD WINAPI TimerThread(LPVOID param); 

// --- Operator Management Functions (Moved from your original utils.h) ---
void addOperator(OperatorNode **root, int id);
void removeOperator(OperatorNode **root, int id);
void operatorStatus(OperatorNode *root);
OperatorNode* linearSearch(OperatorNode *root, int target); // Helper function

// --- Core Routing Function ---
void assignCall(OperatorNode **root, int time);

#endif // ROUTING_H