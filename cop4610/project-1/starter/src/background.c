#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

// Global job tracking
static job_t jobs[MAX_JOBS];
static int next_job_id = 1;

// Initialize job tracking
void init_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        jobs[i].active = 0;
        jobs[i].job_id = 0;
        jobs[i].pid = 0;
        memset(jobs[i].command, 0, MAX_CMD_LEN);
    }
}

// Add a new background job
int add_job(pid_t pid, char *command) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!jobs[i].active) {
            jobs[i].job_id = next_job_id++;
            jobs[i].pid = pid;
            jobs[i].active = 1;
            strncpy(jobs[i].command, command, MAX_CMD_LEN - 1);
            jobs[i].command[MAX_CMD_LEN - 1] = '\0';
            
            // Print job start message
            printf("[%d] %d\n", jobs[i].job_id, pid);
            return jobs[i].job_id;
        }
    }
    return -1; // No available job slots
}

// Remove a completed job
void remove_job(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active && jobs[i].pid == pid) {
            // Print completion message
            printf("[%d]+ done %s\n", jobs[i].job_id, jobs[i].command);
            jobs[i].active = 0;
            break;
        }
    }
}

// Check for completed background processes
void check_background_jobs(void) {
    pid_t pid;
    int status;
    
    // Use WNOHANG to check without blocking
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_job(pid);
    }
}

// Wait for all background jobs to complete (used in exit command)
void wait_for_all_jobs(void) {
    int active_jobs = 0;
    
    // Count active jobs
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active) {
            active_jobs++;
        }
    }
    
    if (active_jobs > 0) {
        printf("Waiting for %d background job(s) to complete...\n", active_jobs);
        
        // Wait for all background processes
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].active) {
                int status;
                waitpid(jobs[i].pid, &status, 0);
                printf("[%d]+ done %s\n", jobs[i].job_id, jobs[i].command);
                jobs[i].active = 0;
            }
        }
    }
}

// Print active jobs (for jobs command)
void print_jobs(void) {
    int found = 0;
    
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active) {
            printf("[%d]+ %d %s\n", jobs[i].job_id, jobs[i].pid, jobs[i].command);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No active background processes.\n");
    }
}

// Check if command should run in background
int is_background_command(tokenlist *tokens) {
    if (tokens->size > 0) {
        char *last_token = tokens->items[tokens->size - 1];
        return (strcmp(last_token, "&") == 0);
    }
    return 0;
}

// Remove the '&' token from the command
void remove_background_token(tokenlist *tokens) {
    if (tokens->size > 0 && strcmp(tokens->items[tokens->size - 1], "&") == 0) {
        free(tokens->items[tokens->size - 1]);
        tokens->items[tokens->size - 1] = NULL;
        tokens->size--;
    }
}

// Execute command in background
void execute_background_command(tokenlist *tokens, char *original_command) {
    pid_t pid;
    
    // Create command string without '&' for job tracking
    char clean_command[MAX_CMD_LEN];
    int pos = 0;
    
    // Build clean command string (without the '&')
    for (int i = 0; i < tokens->size - 1 && pos < MAX_CMD_LEN - 1; i++) {
        if (i > 0) {
            clean_command[pos++] = ' ';
        }
        int len = strlen(tokens->items[i]);
        if (pos + len < MAX_CMD_LEN - 1) {
            strcpy(&clean_command[pos], tokens->items[i]);
            pos += len;
        }
    }
    clean_command[pos] = '\0';
    
    // Remove the '&' from tokens
    remove_background_token(tokens);
    
    pid = fork();
    
    if (pid == 0) {
        // Child process
        
        // For now, simple command execution
        // TODO: Later you'll add I/O redirection and piping here
        
        // Convert tokens to execv format
        char **args = malloc((tokens->size + 1) * sizeof(char*));
        for (int i = 0; i < tokens->size; i++) {
            args[i] = tokens->items[i];
        }
        args[tokens->size] = NULL;
        
        // Simple execution - you'll enhance this with PATH search later
        execv(args[0], args);
        
        // If execv fails, try common paths
        char path_buf[256];
        snprintf(path_buf, sizeof(path_buf), "/bin/%s", args[0]);
        execv(path_buf, args);
        
        snprintf(path_buf, sizeof(path_buf), "/usr/bin/%s", args[0]);
        execv(path_buf, args);
        
        // If we get here, command not found
        fprintf(stderr, "%s: command not found\n", args[0]);
        free(args);
        exit(EXIT_FAILURE);
        
    } else if (pid > 0) {
        // Parent process - add job and continue
        add_job(pid, clean_command);
    } else {
        perror("fork failed");
    }
}