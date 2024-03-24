#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

struct subcommand_t {
    char **argument; // Array of arguments
    size_t size;     // Number of arguments
};

struct command_t {
    struct subcommand_t *subcommand; // Array of subcommands.
    size_t size;                     // Number of subcommands.
};

struct line_t {
    struct command_t *command; // Array of commands.
    size_t size;               // Number of commands.
};

void prompt(char *prompt) {
    #ifdef DEBUG
    fprintf(stderr, "[PROMPT] Displaying prompt\n");
    #endif
    fprintf(stdout, "%s ", prompt);
}

char *reader(size_t size){
    char *buffer;
    #ifdef DEBUG
    fprintf(stderr, "[READER] Allocating memory for buffer [size: %zu]\n", size);
    #endif
    buffer = malloc(sizeof(char) * size);
    if (buffer == NULL) exit(EXIT_FAILURE);
    #ifdef DEBUG
    fprintf(stderr, "[READER] Reading to buffer\n");
    #endif
    int character;
    size_t length = 0;
    while (EOF != (character = fgetc(stdin)) && character != '\n') {
        buffer[length++] = (char) character;
        if (length == size) {
            #ifdef DEBUG
            fprintf(stderr, "[READER] Reallocating memory for buffer [size: %zu]\n", size);
            #endif
            buffer = realloc(buffer, sizeof(char) * (size += 32));
            if (buffer == NULL) exit(EXIT_FAILURE);
        }
    }
    #ifdef DEBUG
    fprintf(stderr, "[READER] Setting the NULL terminator for buffer\n");
    #endif
    buffer[length++] = '\0';
    return realloc(buffer, sizeof(char) * length);
}

void executor(struct line_t line) {
    for (size_t i = 0; i < line.size; ++i) {
        #ifdef DEBUG
        fprintf(stderr, "[EXECUTOR] Executing line.command[%zu]\n", i);
        #endif
        int previous;
        for (size_t j = 0; j < line.command[i].size; ++j) {
            #ifdef DEBUG
            fprintf(stderr, "[EXECUTOR] Executing line.command[%zu].subcommand[%zu]\n", i, j);
            #endif
            int p[2];
            pipe(p);
            if (line.command[i].subcommand[j].size != 0) {
                if (strcmp(line.command[i].subcommand[j].argument[0], "exit") == 0) {
                    #ifdef DEBUG
                    fprintf(stderr, "[EXECUTOR] Exiting shell\n");
                    #endif
                    exit(EXIT_SUCCESS);
                } else if (strcmp(line.command[i].subcommand[j].argument[0], "cd") == 0) {
                    #ifdef DEBUG
                    fprintf(stderr, "[EXECUTOR] Changing directory to %s\n", line.command[i].subcommand[j].argument[1]);
                    #endif
                    chdir(line.command[i].subcommand[j].argument[1]);
                } else {
                    switch(fork()) {
                        case -1:
                            #ifdef DEBUG
                            fprintf(stderr, "[EXECUTOR] Failed to fork a child process\n");
                            #endif
                            exit(EXIT_FAILURE);
                        case  0:
                            #ifdef DEBUG
                            fprintf(stderr, "[EXECUTOR] Running on a child process [pid: %d]\n", getpid());
                            #endif
                            if (j == 0) {
                                dup2(0, STDIN_FILENO);
                            } else {
                                dup2(previous, STDIN_FILENO);
                            }
                            close(p[0]);
                            if (j+1 < line.command[i].size) {
                                dup2(p[1],STDOUT_FILENO);
                            }
                            close(p[0]);
                            execvp(line.command[i].subcommand[j].argument[0], line.command[i].subcommand[j].argument);
                            exit(EXIT_FAILURE);
                        default:
                            #ifdef DEBUG
                            fprintf(stderr, "[EXECUTOR] Running on the parent process [%d]\n", getpid());
                            #endif
                            previous=p[0];
                            close(p[1]);
                    }
                }
            }
        }
        #ifdef DEBUG
        fprintf(stderr, "[EXECUTOR] Waiting for any terminated child processes\n");
        #endif
        while(wait(NULL) > 0) {
            #ifdef DEBUG
            fprintf(stderr, "[EXECUTOR] Found a terminated child process\n");
            #endif
        }
    }
}

struct line_t parser(char *buffer, char *del1, char *del2, char *del3) {
    size_t i, j, k;
    char *str1, *str2, *str3;
    char *token1, *token2, *token3;
    char *saveptr1, *saveptr2, *saveptr3;

    struct line_t line;
    line.size=0;
    line.command=NULL;
    for (i = 0, str1 = buffer ;; i++, str1 = NULL) {
        token1 = strtok_r(str1, del1, &saveptr1);
        if (token1 == NULL) break;
        line.size++;
        if (i == 0) {
            #ifdef DEBUG
            fprintf(stderr, "[PARSER] Allocating memory for line.command [size: %zu]\n", line.size);
            #endif
            line.command = malloc(sizeof(struct command_t));
        } else {
            #ifdef DEBUG
            fprintf(stderr, "[PARSER] Reallocating memory for line.command [size: %zu]\n", line.size);
            #endif
            line.command = realloc(line.command, line.size * sizeof(struct command_t));
        }
        line.command[i].size=0;
        line.command[i].subcommand=NULL;
        for (j = 0, str2 = token1 ;; j++, str2 = NULL) {
            token2 = strtok_r(str2, del2, &saveptr2);
            if (token2 == NULL) break;
            line.command[i].size++;
            if (j == 0) {
                #ifdef DEBUG
                fprintf(stderr, "[PARSER] Allocating memory for line.command[%zu].subcommand [size: %zu]\n", i, line.command[i].size);
                #endif
                line.command[i].subcommand = malloc(sizeof(struct subcommand_t));
            } else {
                #ifdef DEBUG
                fprintf(stderr, "[PARSER] Reallocating memory for line.command[%zu].subcommand [size: %zu]\n", i, line.command[i].size);
                #endif
                line.command[i].subcommand = realloc(line.command[i].subcommand, line.command[i].size * sizeof(struct subcommand_t));
            }
            line.command[i].subcommand[j].size=0;
            line.command[i].subcommand[j].argument=NULL;
            for (k = 0, str3 = token2 ;; k++, str3 = NULL) {
                token3 = strtok_r(str3, del3, &saveptr3);
                if (token3 == NULL) break;
                line.command[i].subcommand[j].size++;
                if (k == 0) {
                    #ifdef DEBUG
                    fprintf(stderr, "[PARSER] Allocating memory for line.command[%zu].subcommand[%zu].argument [size: %zu]\n", i, j, line.command[i].subcommand[j].size);
                    #endif
                    line.command[i].subcommand[j].argument = malloc(sizeof(char *));
                } else {
                    #ifdef DEBUG
                    fprintf(stderr, "[PARSER] Reallocating memory for line.command[%zu].subcommand[%zu].argument [size: %zu]\n", i, j, line.command[i].subcommand[j].size);
                    #endif
                    line.command[i].subcommand[j].argument = realloc(line.command[i].subcommand[j].argument, (line.command[i].subcommand[j].size + 1) * sizeof(char *));
                }
                line.command[i].subcommand[j].argument[k] = malloc((strlen(token3)+1) * sizeof(char));
                memset(line.command[i].subcommand[j].argument[k], 0, strlen(token3)+1);
                strcpy(line.command[i].subcommand[j].argument[k], token3);
            }
            if (line.command[i].subcommand[j].size != 0) {
                #ifdef DEBUG
                fprintf(stderr, "[PARSER] Setting the NULL terminator for line.command[%zu].subcommand[%zu]\n", i, j);
                #endif
                line.command[i].subcommand[j].argument[line.command[i].subcommand[j].size] = NULL;
            }
        }
    }
    return line;
}

int main() {
    while (1) {
        prompt("$");
        char *buffer = reader(1024);
        struct line_t line = parser(buffer, ";", "|", " \t");
        executor(line);
    }
}