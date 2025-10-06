#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "routing.h"
#include <string.h>


int main()
{
    OperatorNode *root = NULL; 
    int choice = 0;
    int Id = 0;
    int time;
    int front = -1, rear=-1;
    // --- Basic authentication ---
    const char requiredUser[] = "admin";
    const int requiredPass = 123;
    char user[64];
    int pass;
    int attempts = 0;
    int authenticated = 0;
    while (attempts < 3) {
        printf("Username: ");
        if (scanf("%63s", user) != 1) break;
        printf("Password (numbers only): ");
        if (scanf("%d", &pass) != 1) { int c; while ((c = getchar()) != '\n' && c != EOF) {} break; }
        if (strcmp(user, requiredUser) == 0 && pass == requiredPass) { authenticated = 1; break; }
        attempts++;
        printf("Invalid credentials. Attempts remaining: %d\n", 3 - attempts);
    }
    if (!authenticated) {
        printf("Authentication failed. Exiting.\n");
        return 1;
    }
    while (1) {
    printf("\nPress 1 to add operator\nPress 2 to remove operator\nPress 3 to see operator status\nPress 4 to assign a call\nPress 5 to display queue status\npress 6 to exit");
           printf("\nEnter choice:");
           scanf("%d",&choice);

        switch (choice)
        {
        case 1:
            printf("Enter operator ID: ");
            if (scanf("%d", &Id) == 1) addOperator(&root, Id);
            else { int c; while ((c = getchar()) != '\n' && c != EOF) {} printf("Invalid ID.\n"); }
            break;
        case 2:
            printf("Enter operator ID: ");
            if (scanf("%d", &Id) == 1) removeOperator(&root, Id);
            else { int c; while ((c = getchar()) != '\n' && c != EOF) {} printf("Invalid ID.\n"); }
            break;
        case 3:
            operatorStatus(root);
            break;
        case 4:
            printf("Enter call timing: ");
            scanf("%d",&time);
            assignCall(&root,time);
            break;
        case 5:
            display();
            break;
        case 6:
            while (root) { OperatorNode *tmp = root; root = root->next; free(tmp); }
            return 0;
        default:
            printf("Please choose a valid option (1-6).\n");
        }
    }
}