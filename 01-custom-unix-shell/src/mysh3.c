#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void run_cmd(char *s) {
    char *argv[64];
    int i = 0;
    char *t = strtok(s, " ");
    while (t != NULL) {
        argv[i++] = t;
        t = strtok(NULL, " ");
    }
    argv[i] = NULL;
    if (argv[0] != NULL) {
        execvp(argv[0], argv);
    }
    exit(1);
}

int main() {
    char buf[256];
    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        buf[strcspn(buf, "\n")] = '\0';
        if (buf[0] == '\0') continue;

        char *p = strchr(buf, '|');
        if (p != NULL) {
            *p = '\0';
            char *left = buf;
            char *right = p + 1;
            int fd[2];
            pipe(fd);

            if (fork() == 0) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]); close(fd[1]);
                run_cmd(left);
            }
            if (fork() == 0) {
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]); close(fd[1]);
                run_cmd(right);
            }
            close(fd[0]); close(fd[1]);
            wait(NULL); wait(NULL);
        } else {
            char *temp = strdup(buf);
            char *first = strtok(temp, " ");
            if (first && strcmp(first, "exit") == 0) {
                free(temp); break;
            }
            if (first && strcmp(first, "cd") == 0) {
                char *arg = strtok(NULL, " ");
                if (arg) chdir(arg);
                free(temp); continue;
            }
            free(temp);
            if (fork() == 0) run_cmd(buf);
            else wait(NULL);
        }
    }
    return 0;
}