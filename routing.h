// routing.h - declarations for operator routing management
#ifndef ROUTING_H
#define ROUTING_H

// Example node structure for operators (can be expanded later)
typedef struct OperatorNode {
	int id;
    int status;
    int callTime;
	struct OperatorNode *next;
} OperatorNode;


// Assign an incoming call to a free operator. Pass pointer to root so function may modify list.
void assignCall(OperatorNode **root, int time);

#endif // ROUTING_H
