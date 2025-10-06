#include<stdio.h>
#include<time.h>
#include <windows.h>
#include"routing.h"
#include"utils.h"



int timer(int duration){
    time_t start = time(NULL);
    while (1) {
        time_t now = time(NULL);
        if (difftime(now, start) >= duration) {
            printf("\nTime's up!");
            break;
        }
    }
    return 1;
}

void addOperator(OperatorNode **root, int id) {
	// Check for duplicate ID first using linearSearch
	if (linearSearch(*root, id) != NULL) {
		printf("\nperator %d already exists. Not adding duplicate.", id);
		return;
	}

	// Append new node
	OperatorNode *node = malloc(sizeof(OperatorNode));
	if (!node) return;
	node->id = id;
	node->status = 0; // default free
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
        if(cur->status==0) printf("\n===>current status is free\n");
        else printf("\n===>current status is busy");
        if(cur->status!=0) printf("\n===>Call duration remaninig: %d seconds\n",cur->callTime);
	}
}

OperatorNode* linearSearch(OperatorNode *root, int target) {
	for (OperatorNode *cur = root; cur; cur = cur->next) {
		if (cur->id == target) return cur;
	}
	return NULL;
}