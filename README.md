# Unix Shell & Multiprocessing in C

Implementations of a custom Unix Command-Line Interpreter (Shell) and multiprocessing coordination mechanisms using POSIX system calls in C. Developed as part of the Operating Systems course at Athens University of Economics and Business (AUEB).

---

## 1. Custom Unix Shell (`shell/`)
An incremental implementation of a Unix Shell developed across three phases:

- **Phase 1 (`mysh1.c`)**:
  - Command parsing and tokenization.
  - Process creation and execution using `fork()`, `execvp()`, and `wait()`.
  
- **Phase 2 (`mysh2.c`)**:
  - Sequential command execution (separated by `;`).
  - Background process execution using `&`.
  - Signal handling for interrupt (`SIGINT`) and child termination (`SIGCHLD`).

- **Phase 3 (`mysh3.c`)**:
  - Standard I/O redirection (`<`, `>`, `>>`).
  - Inter-process communication (IPC) via anonymous pipelines (`|`).

### Compilation & Execution
```bash
gcc -Wall -Wextra -O2 shell/mysh3.c -o mysh
./mysh