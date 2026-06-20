#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "util.h"
 
#define SEM_DISPLAY  "/mp2_display"
#define SEM_CNT_MTX  "/mp2_cnt_mtx"
#define SEM_BARRIER  "/mp2_barrier"
#define SHM_NAME     "/mp2_counter"

int main(int argc, char *argv[])
{
  if (argc < 3) {
        fprintf(stderr, "Usage: %s <repetitions> <string1> [string2 ...]\n", argv[0]);
        exit(1);
    }
 
    int rep = atoi(argv[1]);
    if (rep <= 0) {
        fprintf(stderr, "Error: repetitions must be a positive integer\n");
        exit(1);
    }
 
    int num_strings = argc - 2;

    sem_unlink(SEM_DISPLAY);
    sem_unlink(SEM_CNT_MTX);
    sem_unlink(SEM_BARRIER);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0644);
    if (shm_fd < 0){
      perror("shm_open");
      exit(1);
    }

    if (ftruncate(shm_fd, sizeof(int))<0){
      perror("ftruncate");
      exit(1);
    }
    int *finished_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (finished_count == MAP_FAILED){
      perror("mmap");
      exit(1);
    }
    close(shm_fd);
    *finished_count = 0;

    sem_t *sem_display = sem_open(SEM_DISPLAY, O_CREAT | O_EXCL, 0644, 1);
    if (sem_display == SEM_FAILED){
      perror("sem_open display");
      exit(1);
    }

    sem_t *sem_cnt_mtx = sem_open(SEM_CNT_MTX, O_CREAT | O_EXCL, 0644, 1);
    if (sem_cnt_mtx == SEM_FAILED){
      perror("sem_open cnt_mtx");
      exit(1);
    }

    sem_t *sem_bar = sem_open(SEM_BARRIER, O_CREAT | O_EXCL, 0644, 0);
    if (sem_bar == SEM_FAILED){
      perror("sem_open barrier");
      exit(1);
    }

    for (int i = 0; i < num_strings; i++){
      pid_t pid = fork();
      if (pid < 0 ){
        perror("fork");
        exit(1);
      }

      if (pid == 0) {
        char *str = argv[i +2];
        init();
        sem_wait(sem_cnt_mtx);
        (*finished_count)++;
        int done = (*finished_count == num_strings);
        sem_post(sem_cnt_mtx);

        if (done){
          for (int k=0; k < num_strings; k++){
            sem_post(sem_bar);
          }
        }

        sem_wait(sem_bar);

        for (int j = 0; j < rep; j++){
          sem_wait(sem_display);
          display(str);
          fflush(stdout);
          sem_post(sem_display);
        }

        sem_close(sem_display);
        sem_close(sem_cnt_mtx);
        sem_close(sem_bar);
        munmap(finished_count, sizeof(int));
        exit(0);
      }
    }

    for (int i = 0; i < num_strings; i++){
      wait(NULL);
    }

    sem_close(sem_display);
    sem_close(sem_cnt_mtx);
    sem_close(sem_bar);
    munmap(finished_count, sizeof(int));
    sem_unlink(SEM_DISPLAY);
    sem_unlink(SEM_CNT_MTX);
    sem_unlink(SEM_BARRIER);
    shm_unlink(SHM_NAME);

    return 0;
}