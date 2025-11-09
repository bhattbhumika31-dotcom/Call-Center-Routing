#include <stdio.h>
#include <stdlib.h>
#include "routing.h"
#include "utils.h"
#include <windows.h>
#include <time.h> // Included for potential use, though Sleep is used

// --- Global Queue Variables (Removed from here, now in utils.c) ---

// --- Timer Thread Implementation (Modified to check the queue) ---
DWORD WINAPI TimerThread(LPVOID param) 
{
    OperatorNode *node = (OperatorNode*)param;
    if (!node) return 0;
    int duration = node->callTime;
    Sleep(duration * 1000);
    node->status = 0;
    node->callTime = 0;
    
    // Check if the queue has calls and reassign immediately
    if (isQueueNotEmpty()) {
        printf("\nOperator %d is now FREE. Checking queue for waiting calls...\n", node->id);
        attempt_reassign_from_queue(node); 
    } else {
        printf("\nOperator %d is now FREE. No waiting calls.\n", node->id);
    }
    return 0;
}

// --- Core Routing Function ---

void assignCall(OperatorNode **root,int time) {
    if (!root || !*root) {
        printf("\nNo operators available to assign the call.");
        return;
    }
    for (OperatorNode *cur = *root; cur; cur = cur->next) {
        if (cur->status == 0) {
            cur->status = 1; // busy
            cur->callTime = time; 
            printf("\nAssigned call to operator %d (duration %d seconds)\n", cur->id, cur->callTime);
            HANDLE hThread = CreateThread(NULL, 0, TimerThread, cur, 0, NULL);
            if (hThread) CloseHandle(hThread);
            return;
        }
    }
    // Call enqueue (now defined in utils.c)
    enqueue(time);
    printf("\nAll operators are busy; call duration %d enqueued.", time);
}

// --- Operator Management Functions (Moved from your original utils.c block) ---

OperatorNode* linearSearch(OperatorNode *root, int target) {
    for (OperatorNode *cur = root; cur; cur = cur->next) {
        if (cur->id == target) return cur;
    }
    return NULL;
}

void addOperator(OperatorNode **root, int id) {
    if (linearSearch(*root, id) != NULL) {
        printf("\nOperator %d already exists. Not adding duplicate.", id);
        return;
    }

    OperatorNode *node = malloc(sizeof(OperatorNode));
    if (!node) return;
    node->id = id;
    node->status = 0; 
    node->callTime = 0;
    node->next = NULL;
    if (*root == NULL) {
        *root = node;
    } else {
        OperatorNode *cur = *root;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
    printf("\nAdded operator %d\n", id);
}

void removeOperator(OperatorNode **root, int id) {
    OperatorNode *cur = *root;
    OperatorNode *prev = NULL;
    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next;
            else *root = cur->next;
            free(cur);
            printf("\nRemoved operator %d", id);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    printf("\nOperator %d not found", id);
}

void operatorStatus(OperatorNode *root) {
    if (!root) {
        printf("\nNo operators present.");
        return;
    }
    printf("Operator list:\n");
    for (OperatorNode *cur = root; cur; cur = cur->next) {
        printf("\n[+] Showing details of operator  %d", cur->id);
        if(cur->status==0) printf("\n===>Current status is FREE\n");
        else {
            printf("\n===>Current status is BUSY");
            printf("\n===>Call duration remaining: %d seconds\n",cur->callTime);
        }
    }
}