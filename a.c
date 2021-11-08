#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>

#define BUF_SIZE 1000
#define SHARED_MEMORY_NAME "/shm_file"
#define FIRST_SEM "/sem1"
#define SECOND_SEM "/sem2"
#define THIRD_SEM "/sem3"

int main() {
	
	


	int fd_shared;
	char* shmem;
	char* tmp = (char*)malloc(sizeof(char) * BUF_SIZE);
	char* buf_size = (char*)malloc(sizeof(char) * 10);


	sem_t* sem1 = sem_open(FIRST_SEM, O_CREAT, 0666, 0);
	sem_t* sem2 = sem_open(SECOND_SEM, O_CREAT, 0666, 0);
	sem_t* sem3 = sem_open(THIRD_SEM, O_CREAT, 0666, 0);
	if (sem1 == SEM_FAILED || sem2 == SEM_FAILED || sem3 == SEM_FAILED) {
		perror("Semaphore opening error, program 'a'\n");
		exit(1);
	}


	if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
		perror("shared memory unlink error\n");
		exit(1);
	}
	
	if ((fd_shared = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1) {
		perror("shared memory open error, program 'a'\n");
		exit(1);
	}

	if (ftruncate(fd_shared, BUF_SIZE) == -1) {
		perror("allocating memory error, program 'a'\n");
		exit(-1);
	}
	
	shmem = (char*)mmap(NULL, BUF_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd_shared, 0);	
	sprintf(buf_size, "%d", BUF_SIZE);
	char* argv[] = { buf_size, SHARED_MEMORY_NAME, SECOND_SEM, THIRD_SEM, NULL };


  	while (scanf ("%s", tmp)) {

  		pid_t p = fork();
  		if (p == 0) {
  				pid_t p_1 = fork();
  				if (p_1 == 0) {
  					sem_wait(sem1);
  					printf("number of characters sent by the program A:\n");
  					if (execve("./b.out", argv, NULL) == -1) {
  						perror("Could not execve, program 'a'\n");
  					}
  				} else if (p_1 > 0) {
  					sem_wait(sem3);
					if (execve("./c.out", argv, NULL) == -1) {
						perror("Could not execve, program 'a'\n");
					}
  				}
				
		
		} else if (p > 0) {
			
			sprintf(shmem, "%s", tmp);
  			sem_post(sem1);
  			sem_wait(sem2);
  			printf("/*______________________*/\n\n");
  			
		}	
		
  	}
  	
	shm_unlink(SHARED_MEMORY_NAME);
  	sem_unlink(FIRST_SEM);
	sem_unlink(SECOND_SEM);
	sem_unlink(THIRD_SEM);
	sem_close(sem1);
	sem_close(sem2);
	sem_close(sem3);
	close(fd_shared);
	
}