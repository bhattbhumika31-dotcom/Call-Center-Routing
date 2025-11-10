#include <stdio.h>
#include <stdlib.h>
#include "routing.h"
#include "utils.h"
#include <windows.h>
#include <string.h>
#include <time.h> 

// --- Timer Thread Implementation (Modified to check the queue) ---
DWORD WINAPI TimerThread(LPVOID param) 
{
    OperatorNode *node = (OperatorNode*)param;
    if (!node) return 0;
    int duration = node->callTime;
    Sleep(duration * 1000);
    node->status = 0;
    node->callTime = 0;
    
    // Attempt to reassign a queued call to this now-free operator.
    // attempt_reassign_from_queue will dequeue and start a timer if assignment occurs.
    if (attempt_reassign_from_queue(node)) {
        // assignment message is printed inside attempt_reassign_from_queue
    } else {
        printf("\nOperator %d (%s) is now FREE. No waiting calls.\n", node->id, node->name);
    }
    return 0;
}

// --- BST Helper Functions ---

static OperatorNode* find_min(OperatorNode *node) {
    if (!node) return NULL;
    while (node->left) node = node->left;
    return node;
}

static OperatorNode* insert_node(OperatorNode *root, int id, const char *name) {
    if (!root) {
        OperatorNode *node = malloc(sizeof(OperatorNode));
        if (!node) return NULL;
        node->id = id;
        node->status = 0;
        node->callTime = 0;
        if (name) {
            strncpy(node->name, name, sizeof(node->name) - 1);
            node->name[sizeof(node->name) - 1] = '\0';
        } else node->name[0] = '\0';
        node->left = node->right = NULL;
        return node;
    }
    if (id < root->id) root->left = insert_node(root->left, id, name);
    else if (id > root->id) root->right = insert_node(root->right, id, name);
    else printf("\nOperator %d already exists. Not adding duplicate.", id);
    return root;
}

static OperatorNode* delete_node(OperatorNode *root, int id) {
    if (!root) return NULL;
    if (id < root->id) root->left = delete_node(root->left, id);
    else if (id > root->id) root->right = delete_node(root->right, id);
    else {
        // found node to delete
        if (!root->left) {
            OperatorNode *r = root->right;
            free(root);
            return r;
        } else if (!root->right) {
            OperatorNode *l = root->left;
            free(root);
            return l;
        } else {
            OperatorNode *succ = find_min(root->right);
            // copy successor data into root
            root->id = succ->id;
            strncpy(root->name, succ->name, sizeof(root->name) - 1);
            root->name[sizeof(root->name) - 1] = '\0';
            root->status = succ->status;
            root->callTime = succ->callTime;
            // delete successor node
            root->right = delete_node(root->right, succ->id);
        }
    }
    return root;
}

// Public: search by id using DFS (preorder)
OperatorNode* DFS(OperatorNode *root, int target) {
    if (!root) return NULL;
    if (root->id == target) return root;
    OperatorNode *res = DFS(root->left, target);
    if (res) return res;
    return DFS(root->right, target);
}

void addOperator(OperatorNode **root, int id, const char *name) {
    if (!root) return;
    if (DFS(*root, id) != NULL) {
        printf("\nOperator %d already exists. Not adding duplicate.", id);
        return;
    }
    *root = insert_node(*root, id, name);
    OperatorNode *just = DFS(*root, id);
    if (just) printf("\nAdded operator %d (%s)\n", id, just->name);
}

void removeOperator(OperatorNode **root, int id) {
    if (!root || !*root) { printf("\nOperator %d not found", id); return; }
    OperatorNode *found = DFS(*root, id);
    if (!found) { printf("\nOperator %d not found", id); return; }
    char saved_name[64];
    strncpy(saved_name, found->name, sizeof(saved_name) - 1);
    saved_name[sizeof(saved_name) - 1] = '\0';
    *root = delete_node(*root, id);
    printf("\nRemoved operator %d (%s)", id, saved_name);
}

// find the first FREE operator using inorder traversal (smallest id free)
OperatorNode* find_free_operator(OperatorNode *root) {
    if (!root) return NULL;
    OperatorNode *left = find_free_operator(root->left);
    if (left) return left;
    if (root->status == 0) return root;
    return find_free_operator(root->right);
}

void assignCall(OperatorNode **root,int time) {
    if (!root || !*root) {
        printf("\nNo operators available to assign the call.");
        return;
    }
    OperatorNode *free_op = find_free_operator(*root);
    if (free_op) {
        free_op->status = 1;
        free_op->callTime = time;
        printf("\nAssigned call to operator %d (%s) (duration %d seconds)\n", free_op->id, free_op->name, free_op->callTime);
        HANDLE hThread = CreateThread(NULL, 0, TimerThread, free_op, 0, NULL);
        if (hThread) CloseHandle(hThread);
        return;
    }
    enqueue(time);
    printf("\nAll operators are busy; call duration %d enqueued.", time);
}

// inorder helper that prints nodes (no header)
static void operatorStatus_inorder(OperatorNode *root) {
    if (!root) return;
    operatorStatus_inorder(root->left);
    printf("\n[+] Showing details of operator  %d (%s)", root->id, root->name);
    if(root->status==0) printf("\n===>Current status is FREE\n");
    else {
        printf("\n===>Current status is BUSY");
        printf("\n===>Call duration remaining: %d seconds\n",root->callTime);
    }
    operatorStatus_inorder(root->right);
}

void operatorStatus(OperatorNode *root) {
    if (!root) {
        printf("\nNo operators present.");
        return;
    }
    printf("Operator list:\n");
    operatorStatus_inorder(root);
}

void free_tree(OperatorNode *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
