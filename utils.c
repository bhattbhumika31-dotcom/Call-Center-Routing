#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "routing.h"
#include <windows.h>

#define MAX 100 // Defined here and in utils.h

// Global Queue Variables (Externally declared in utils.h)
int queue[MAX];
int front = -1;
int rear = -1;

// --- Queue Operations (Copied from your original routing.c block) ---

int isQueueNotEmpty() {
    return front != -1;
}

void enqueue(int value) 
{
    if ((front == 0 && rear == MAX - 1) || ((rear + 1) % MAX == front)) {
        printf("\nQueue Overflow! Cannot insert %d", value);
        return;
    }
    if (front == -1) {
        front = 0;
        rear = 0;
    }
    else {
        rear = (rear + 1) % MAX;
    }

    queue[rear] = value;
    printf("\n%d enqueued to the queue.", value);
}

int dequeue() {
    int value = -1; 
    if (front == -1) {
        // printf("\nQueue Underflow! Cannot dequeue."); // Removed loud error for internal use
        return value;
    }

    value = queue[front];
    printf("\n%d dequeued from the queue.", value); 
    
    if (front == rear) {
        front = -1;
        rear = -1;
    }
    else {
        front = (front + 1) % MAX;
    }
    return value;
}

void display() {
    if (front == -1) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue elements: ");
    int i = front;
    while (1) {
        printf("%d ", queue[i]);
        if (i == rear)
            break;
        i = (i + 1) % MAX;
    }
    printf("\n");
}

// --- Reassignment Logic ---

// Called by TimerThread in routing.c when an operator becomes free
// Returns 1 if a queued call was assigned to freed_operator, 0 otherwise
int attempt_reassign_from_queue(OperatorNode *freed_operator) {
    if (!freed_operator || freed_operator->status == 1) {
        return 0;
    }
    
    int call_time = dequeue();
    
    if (call_time != -1) {
        // Assign the dequeued call to the newly freed operator
        freed_operator->status = 1; // busy
        freed_operator->callTime = call_time; 
        printf("\n[QUEUE REROUTE] Assigned call from queue (duration %d) to operator %d (%s)\n", call_time, freed_operator->id, freed_operator->name);
        
        // Start a new timer thread for this operator with the new call time
        HANDLE hThread = CreateThread(NULL, 0, TimerThread, freed_operator, 0, NULL);
        if (hThread) CloseHandle(hThread);
        return 1;
    }
    return 0;
}