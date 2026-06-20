#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char buf[256];

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            break;
        }

        buf[strcspn(buf, "\n")] = '\0';

        char *cmd = strtok(buf, " \t");

        if (cmd == NULL) {
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        pid_t pid = fork();
        if (pid == 0) {
            char *argv[] = {cmd, NULL};
            execvp(cmd, argv);
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        }
    }

    return 0;
}