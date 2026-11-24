#include "lexer.h"
#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Store last valid commands for exit command
#define MAX_HISTORY 3
static char command_history[MAX_HISTORY][MAX_CMD_LEN];
static int history_count = 0;

void display_prompt(void) {
    // For now, just use simple prompt. You can enhance this later for Part 1
    // to show USER@MACHINE:PWD>
    printf("> ");
    fflush(stdout);
}

// Add command to history (for exit command)
void add_to_history(char *command) {
    if (command == NULL || strlen(command) == 0) {
        return;
    }
    
    // Shift existing commands down
    for (int i = MAX_HISTORY - 1; i > 0; i--) {
        strcpy(command_history[i], command_history[i-1]);
    }
    
    // Add new command at the beginning
    strncpy(command_history[0], command, MAX_CMD_LEN - 1);
    command_history[0][MAX_CMD_LEN - 1] = '\0';
    
    if (history_count < MAX_HISTORY) {
        history_count++;
    }
}

// Print command history (for exit command)
void print_command_history(void) {
    if (history_count == 0) {
        printf("No valid commands found.\n");
        return;
    }
    
    printf("Last %d valid command(s):\n", history_count);
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, command_history[i]);
    }
}

// Placeholder function for foreground command execution
void execute_foreground_command(tokenlist *tokens) {
    printf("Executing foreground command: %s\n", tokens->items[0]);
    // TODO: This is where you'll implement:
    // - Environment variable expansion (Part 2)
    // - Tilde expansion (Part 3)
    // - PATH search (Part 4)
    // - External command execution (Part 5)
    // - I/O redirection (Part 6)
    // - Piping (Part 7)
}

// Implementation of cd command
void handle_cd_command(tokenlist *tokens) {
    char *target_dir = NULL;
    
    // Check number of arguments
    if (tokens->size > 2) {
        fprintf(stderr, "cd: too many arguments\n");
        return;
    }
    
    // If no arguments, change to $HOME
    if (tokens->size == 1) {
        target_dir = getenv("HOME");
        if (target_dir == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            return;
        }
    } else {
        target_dir = tokens->items[1];
    }
    
    // Attempt to change directory
    if (chdir(target_dir) != 0) {
        perror("cd");
        return;
    }
    
    printf("Changed directory to: %s\n", target_dir);
}

// Implementation of exit command
void handle_exit_command(tokenlist *tokens) {
    printf("Exiting shell...\n");
    
    // Display last three valid commands
    print_command_history();
}

// Check if a command is a built-in command
int is_builtin_command(char *command) {
    return (strcmp(command, "exit") == 0 ||
            strcmp(command, "cd") == 0 ||
            strcmp(command, "jobs") == 0);
}

int main() {
    // Initialize background job tracking
    init_jobs();
    
    // Initialize command history
    for (int i = 0; i < MAX_HISTORY; i++) {
        memset(command_history[i], 0, MAX_CMD_LEN);
    }
    
    printf("Shell started. Type 'exit' to quit.\n");
    
    while (1) {
        // Check for completed background jobs at the start of each loop
        check_background_jobs();
        
        // Display prompt
        display_prompt();
        
        // Get user input
        char *input = get_input();
        
        // Handle empty input or EOF
        if (input == NULL || strlen(input) == 0) {
            if (input) free(input);
            continue;
        }
        
        // For debugging - you can remove these later
        printf("whole input: %s\n", input);
        
        // Tokenize input
        tokenlist *tokens = get_tokens(input);
        
        // Debug output - you can remove this later
        for (int i = 0; i < tokens->size; i++) {
            printf("token %d: (%s)\n", i, tokens->items[i]);
        }
        
        // Skip empty token lists
        if (tokens->size == 0) {
            free(input);
            free_tokens(tokens);
            continue;
        }
        
        // Handle built-in commands
        if (strcmp(tokens->items[0], "exit") == 0) {
            // Wait for all background jobs before exiting
            wait_for_all_jobs();
            
            // Handle exit command
            handle_exit_command(tokens);
            
            free(input);
            free_tokens(tokens);
            break;
            
        } else if (strcmp(tokens->items[0], "jobs") == 0) {
            print_jobs();
            
        } else if (strcmp(tokens->items[0], "cd") == 0) {
            handle_cd_command(tokens);
            
        } else {
            // External command - add to history if it's not a built-in
            add_to_history(input);
            
            // Check if it should run in background
            if (is_background_command(tokens)) {
                printf("Background command detected: %s\n", tokens->items[0]);
                execute_background_command(tokens, input);
            } else {
                printf("Foreground command detected: %s\n", tokens->items[0]);
                execute_foreground_command(tokens);
            }
        }
        
        free(input);
        free_tokens(tokens);
    }
    
    printf("Shell terminated.\n");
    return 0;
}