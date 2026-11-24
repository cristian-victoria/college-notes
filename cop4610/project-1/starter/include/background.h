#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <sys/types.h>
#include "lexer.h"

#define MAX_JOBS 10
#define MAX_CMD_LEN 200

// Structure to track background jobs
typedef struct {
    int job_id;
    pid_t pid;
    char command[MAX_CMD_LEN];
    int active;
} job_t;

// Function declarations
void init_jobs(void);
int add_job(pid_t pid, char *command);
void remove_job(pid_t pid);
void check_background_jobs(void);
void wait_for_all_jobs(void);
void print_jobs(void);
int is_background_command(tokenlist *tokens);
void remove_background_token(tokenlist *tokens);
void execute_background_command(tokenlist *tokens, char *original_command);

#endif /* BACKGROUND_H */