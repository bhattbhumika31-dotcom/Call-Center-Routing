#ifndef UTILS_H
#define UTILS_H

#include "routing.h" // Needed for OperatorNode structure in reassignment function

#define MAX 100 

// --- Queue Function Declarations ---
void enqueue(int value);
// Modified dequeue to return the value (or -1) for cleaner reassignment logic
int dequeue(); 
void display();
int isQueueNotEmpty(); 

// --- Queue Reassignment Logic ---
void attempt_reassign_from_queue(OperatorNode *freed_operator); 

#endif // UTILS_H