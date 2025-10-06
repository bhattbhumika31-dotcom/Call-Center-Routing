#include <stdio.h>
#include <stdlib.h>
#include "routing.h"
#include "utils.h"
#include <windows.h>
#define MAX 100


int queue[MAX];
int front,rear;
front=-1;
rear=-1;

DWORD WINAPI TimerThread(LPVOID param) {
	OperatorNode *node = (OperatorNode*)param;
	if (!node) return 0;
	int duration = node->callTime;
	Sleep(duration * 1000);
	// mark operator free when timer finishes
	node->status = 0;
	node->callTime = 0;
	return 0;
}

// Assign a call to the first free operator (status == 0). If none free, print message.
void assignCall(OperatorNode **root,int time) {
	if (!root || !*root) {
		printf("\nNo operators available to assign the call.");
		return;
	}
	for (OperatorNode *cur = *root; cur; cur = cur->next) {
		if (cur->status == 0) {
			cur->status = 1; // busy
			cur->callTime = time; // example default duration
			printf("\nAssigned call to operator %d (duration %d seconds)\n", cur->id, cur->callTime);
			// start a timer thread that will mark this operator free after callTime seconds
			HANDLE hThread = CreateThread(NULL, 0, TimerThread, cur, 0, NULL);
			if (hThread) CloseHandle(hThread);
			return;
		}
	}
    enqueue(time);
	printf("\nAll operators are busy; call not assigned.");
}


void enqueue(int value) {
    // Check if queue is full
    if ((front == 0 && rear == MAX - 1) || ((rear + 1) % MAX == front)) {
        printf("\nQueue Overflow! Cannot insert %d", value);
        return;
    }

    // First insertion
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

void dequeue() {
    if (front == -1) {
        printf("\nQueue Underflow! Cannot dequeue.");
        return;
    }

    printf("\n%d dequeued from the queue.", queue[front]);
    if (front == rear) {
        front = -1;
        rear = -1;
    }
    else {
        front = (front + 1) % MAX;
    }
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