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
// Attempts to assign a waiting call to the freed operator.
// Returns 1 if a call was assigned, 0 if queue was empty or not assigned.
int attempt_reassign_from_queue(OperatorNode *freed_operator); 

#endif // UTILS_H