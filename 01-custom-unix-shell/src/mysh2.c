#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char *buf = NULL;
    size_t buf_size = 0;
    ssize_t line_len;

    while (1) {
        printf("$ ");
        fflush(stdout);

        line_len = getline(&buf, &buf_size, stdin);
        if (line_len == -1) {
            break;
        }

        if (line_len > 0 && buf[line_len - 1] == '\n') {
            buf[line_len - 1] = '\0';
        }

        if (buf[0] == '\0') {
            continue;
        }

        int max_args = 16;
        char **args = malloc(max_args * sizeof(char *));
        if (args == NULL) {
            perror("malloc failed");
            continue;
        }

        int i = 0;
        char *token = strtok(buf, " \t");
        while (token != NULL) {
            if (i >= max_args - 1) {
                max_args *= 2;
                char **new_args = realloc(args, max_args * sizeof(char *));
                if (new_args == NULL) {
                    perror("realloc failed");
                    break;
                }
                args = new_args;
            }

            args[i] = strdup(token);
            if (args[i] == NULL) {
                perror("strdup failed");
                break;
            }
            i++;
            token = strtok(NULL, " \t");
        }
        args[i] = NULL;

        if (i > 0) {
            if (strcmp(args[0], "exit") == 0) {
                int j;
                for (j = 0; j < i; j++) {
                    free(args[j]);
                }
                free(args);
                free(buf);
                exit(0);
            }

            if (strcmp(args[0], "cd") == 0) {
                if (args[1] != NULL) {
                    if (chdir(args[1]) != 0) {
                        perror("cd failed");
                    }
                }
            } else {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork failed");
                } else if (pid == 0) {
                    execvp(args[0], args);
                    perror("execvp failed");
                    exit(1);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
        }

        int j;
        for (j = 0; j < i; j++) {
            free(args[j]);
        }
        free(args);
    }

    free(buf);
    return 0;
}