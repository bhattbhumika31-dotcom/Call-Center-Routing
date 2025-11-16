#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "routing.h"

// --- NEW CONSTANTS AND PROTOTYPES ---
#define CREDENTIALS_FILE "credentials.txt"
#define PENDING_FILE "pending_requests.txt"
#define ADMIN_USER "admin"
#define ADMIN_PASS 123
#define MAX_USERNAME 64
#define MAX_PASSWORD 64

// New Access Control Functions
void handle_register();
int handle_login(char *username);
void handle_admin_approval();
int check_credentials(const char *username, const char *password, const char *filename);
// Original Main Logic Encapsulated
void run_call_center_system(); 

char current_user[MAX_USERNAME] = {0}; // To store the logged-in user

// --- MAIN FUNCTION (MODIFIED) ---

int main()
{
    int choice;
    
    // This is the new main access loop, replacing the old hardcoded admin login.
    while (1) {
        printf("\n====================================\n");
        printf("| CALL CENTER ACCESS SYSTEM |\n");
        printf("====================================\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Admin Approval (Admin Only)\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                if (handle_login(current_user)) {
                    // Successful login. Launch the core system.
                    run_call_center_system();
                }
                break;
            case 2:
                handle_register();
                break;
            case 3:
                handle_admin_approval();
                break;
            case 4:
                printf("Exiting system.\n");
                return 0;
            default:
                printf("Invalid choice. Please choose 1-4.\n");
        }
    }
    return 0;
}

// --- NEW FEATURE IMPLEMENTATION (Login/Register/Admin) ---

void handle_register() {
    char username[MAX_USERNAME], password[MAX_PASSWORD], confirm_pass[MAX_PASSWORD];
    
    printf("\n--- New User Registration ---\n");
    printf("Enter Username: ");
    scanf(" %63s", username);
    printf("Enter Password: ");
    scanf(" %63s", password);
    printf("Confirm Password: ");
    scanf(" %63s", confirm_pass);

    if (strcmp(password, confirm_pass) != 0) {
        printf("Error: Passwords do not match.\n");
        return;
    }

    if (check_credentials(username, "", CREDENTIALS_FILE) || check_credentials(username, "", PENDING_FILE)) {
        printf("Error: Username already exists or is pending approval.\n");
        return;
    }

    FILE *fp = fopen(PENDING_FILE, "a");
    if (fp == NULL) {
        perror("Error opening pending file");
        return;
    }
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);

    printf("Registration successful. Your request is pending admin approval.\n");
}

int handle_login(char *username) {
    char password[MAX_PASSWORD];

    printf("\n--- User Login ---\n");
    printf("Username: ");
    scanf(" %63s", username);
    printf("Password: ");
    scanf(" %63s", password);

    if (check_credentials(username, password, CREDENTIALS_FILE)) {
        printf("\nLogin successful. Welcome, %s!\n", username);
        return 1;
    } else {
        printf("\nLogin failed: Invalid username or password, or user not approved.\n");
        return 0;
    }
}

int check_credentials(const char *username, const char *password, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0; 
    }
    
    char file_user[MAX_USERNAME];
    char file_pass[MAX_PASSWORD];
    
    while (fscanf(fp, "%63s %63s", file_user, file_pass) == 2) {
        if (strcmp(username, file_user) == 0) {
            if (password[0] == '\0' || strcmp(password, file_pass) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }
    
    fclose(fp);
    return 0;
}

void handle_admin_approval() {
    char admin_user[MAX_USERNAME];
    int admin_pass;
    int attempts = 0;
    
    printf("\n--- Admin Authentication ---\n");
    while (attempts < 3) {
        printf("Admin Username: ");
        scanf(" %63s", admin_user);
        printf("Admin Password (numbers only): ");
        if (scanf("%d", &admin_pass) != 1) { 
             int c; while ((c = getchar()) != '\n' && c != EOF) {} break; 
        }

        if (strcmp(admin_user, ADMIN_USER) == 0 && admin_pass == ADMIN_PASS) {
            break; 
        }
        attempts++;
        printf("Invalid admin credentials. Attempts remaining: %d\n", 3 - attempts);
    }

    if (attempts == 3) {
        printf("Authentication failed. Returning to main menu.\n");
        return;
    }
    
    printf("\nAdmin Access Granted. Checking pending requests...\n");
    
    FILE *fp_pending = fopen(PENDING_FILE, "r");
    if (fp_pending == NULL) {
        printf("No pending requests found.\n");
        return;
    }
    
    FILE *fp_approved = fopen(CREDENTIALS_FILE, "a");
    if (fp_approved == NULL) {
        perror("Error opening approved file for writing");
        if (fp_pending) fclose(fp_pending);
        return;
    }

    char username[MAX_USERNAME], password[MAX_PASSWORD];
    char approval_choice;
    int request_count = 0;

    while (fscanf(fp_pending, "%63s %63s", username, password) == 2) {
        request_count++;
        printf("\nPending Request #%d:\n", request_count);
        printf("Username: %s\n", username);
        printf("Approve (y/n)? ");
        scanf(" %c", &approval_choice); 

        if (approval_choice == 'y' || approval_choice == 'Y') {
            fprintf(fp_approved, "%s %s\n", username, password);
            printf("User %s approved and moved to credentials.\n", username);
        } else {
            printf("User %s denied.\n", username);
        }
    }

    if (request_count == 0) {
        printf("No new pending requests found.\n");
    }

    fclose(fp_pending);
    fclose(fp_approved);

    remove(PENDING_FILE); 
}

// --- ORIGINAL CALL CENTER SYSTEM (Copied from your previous main.c) ---

void run_call_center_system() {
    OperatorNode *root = NULL; 
    int choice = 0;
    int Id = 0;
    int call_time;
    
    printf("\n------------------------------------------\n");
    printf("|  CALL CENTER ROUTING SYSTEM  |\n");
    printf("|  U-S-E-R A-C-C-E-S-S G-R-A-N-T-E-D (User: %s) |\n", current_user);
    printf("------------------------------------------\n");

    while (1) {
        printf("\nPress 1 to add operator\nPress 2 to remove operator\nPress 3 to see operator status\nPress 4 to assign a call\nPress 5 to display queue status\npress 6 to exit");
        printf("\nEnter choice:");
        if (scanf("%d",&choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Enter operator ID: ");
            if (scanf("%d", &Id) == 1) {
                char name[64];
                printf("Enter operator name: ");
                scanf(" %63s", name);
                addOperator(&root, Id, name);
            }
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
            if (scanf("%d", &call_time) == 1) {
                assignCall(&root, call_time);
            } else { int c; while ((c = getchar()) != '\n' && c != EOF) {} printf("Invalid timing.\n"); }
            break;
        case 5:
            display();
            break;
        case 6:
            free_tree(root);
            root = NULL;
            printf("\nLogged out. Returning to access menu.\n");
            return; 
        default:
            printf("Please choose a valid option (1-6).\n");
        }
    }
}