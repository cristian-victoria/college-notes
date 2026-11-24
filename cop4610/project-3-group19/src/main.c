#include "lexer.h"
#include "fat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global file system pointer
FAT32FileSystem *g_fs = NULL;

// Function declarations
void print_prompt(void);
void handle_command(tokenlist *tokens);
void handle_info(void);
void handle_exit(void);
void handle_ls(void);
void handle_cd(tokenlist *tokens);
void handle_mkdir(tokenlist *tokens);
void handle_creat(tokenlist *tokens);

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <FAT32_image_file>\n", argv[0]);
        return 1;
    }

    // Mount the FAT32 image
    g_fs = mount_fat32_image(argv[1]);
    if (!g_fs) {
        fprintf(stderr, "Failed to mount image file.\n");
        return 1;
    }

    printf("Successfully mounted '%s'\n", g_fs->image_name);
    printf("Type 'ls' to list directory, 'cd' to change directory, 'mkdir' to create a new director, 'creat' to create a new file, 'info' for info, 'exit' to quit.\n\n");

    // Main command loop
    while (1) {
        print_prompt();

        char *input = get_input();
        
        // Skip empty input
        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        tokenlist *tokens = get_tokens(input);
        
        // Handle the command
        if (tokens->size > 0) {
            handle_command(tokens);
        }

        free(input);
        free_tokens(tokens);
    }

    return 0;
}

// Prints the shell prompt
void print_prompt(void) {
    if (g_fs) {
        printf("[%s]%s> ", g_fs->image_name, g_fs->current_path);
    } else {
        printf("> ");
    }
    fflush(stdout);
}

// Handles user commands
void handle_command(tokenlist *tokens) {
    if (tokens->size == 0) {
        return;
    }

    char *cmd = tokens->items[0];

    if (strcmp(cmd, "info") == 0) {
        handle_info();
    } else if (strcmp(cmd, "exit") == 0) {
        handle_exit();
    } else if (strcmp(cmd, "ls") == 0) {
        handle_ls();
    } else if (strcmp(cmd, "cd") == 0) {
        handle_cd(tokens);
    } else if (strcmp(cmd, "mkdir") == 0) {
        handle_mkdir(tokens);
    } else if (strcmp(cmd, "creat") == 0) {
        handle_creat(tokens);
    } else {
        printf("Error: Unknown command '%s'\n", cmd);
        printf("Available commands: info, ls, cd, mkdir, creat, exit\n");
    }
}

// Handles the 'info' command - prints boot sector information
void handle_info(void) {
    if (!g_fs) {
        fprintf(stderr, "Error: No file system mounted.\n");
        return;
    }

    print_info(g_fs);
}

// Handles the 'exit' command - safely closes program
void handle_exit(void) {
    printf("Unmounting file system and exiting...\n");
    
    // Unmount and free resources
    if (g_fs) {
        unmount_fat32_image(g_fs);
        g_fs = NULL;
    }
    
    exit(0);
}

// Handles the 'ls' command - lists directory contents
void handle_ls(void) {
    if (!g_fs) {
        fprintf(stderr, "Error: No file system mounted.\n");
        return;
    }

    list_directory(g_fs);
}

// Handles the 'cd' command - changes directory
void handle_cd(tokenlist *tokens) {
    if (!g_fs) {
        fprintf(stderr, "Error: No file system mounted.\n");
        return;
    }

    // Check if directory name was provided
    if (tokens->size < 2) {
        fprintf(stderr, "Error: cd requires a directory name.\n");
        fprintf(stderr, "Usage: cd <directory>\n");
        return;
    }

    // Change to the specified directory
    change_directory(g_fs, tokens->items[1]);
}

// Handles the 'mkdir' command - creates a new directory
void handle_mkdir(tokenlist *tokens) {
    if (!g_fs) {
        fprintf(stderr, "Error: No file system mounted.\n");
        return;
    }

    // Check if directory name was provided
    if (tokens->size < 2) {
        fprintf(stderr, "Error: mkdir requires a directory name.\n");
        fprintf(stderr, "Usage: mkdir <directory>\n");
        return;
    }

    // Create the directory
    if (make_directory(g_fs, tokens->items[1])) {
        printf("Directory '%s' created successfully.\n", tokens->items[1]);
    }
}

// Handles the 'creat' command - creates a new file
void handle_creat(tokenlist *tokens) {
    if (!g_fs) {
        fprintf(stderr, "Error: No file system mounted.\n");
        return;
    }

    // Check if filename was provided
    if (tokens->size < 2) {
        fprintf(stderr, "Error: creat requires a filename.\n");
        fprintf(stderr, "Usage: creat <filename>\n");
        return;
    }

    // Create the file
    if (create_file(g_fs, tokens->items[1])) {
        printf("File '%s' created successfully.\n", tokens->items[1]);
    }
}