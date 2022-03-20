#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char* argv[])
{
	pid_t pid;
	int n;
	int fd[2];
	printf("Please input the length n: ");
	scanf("%d", &n);

	//create the pipe
	if (pipe(fd) == -1){
		fprintf(stderr, "Pipe failed");
		return -1;
	}

	// create child process
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Fork Failed");
		return 1;
	}

	else if (pid == 0) {    //child process
		//close the read end since we're the producer
		close(fd[READ_END]);

		//random number seed
		double a;
		srand((unsigned)time(NULL));

        float now;
		//write the array to the pipe
		for (int i=0; i<n; i++){
			a = (double)rand() / RAND_MAX * 4.999;   //get random time interval
			now = pow(0.5 * i, 2);
			write(fd[WRITE_END], &now, sizeof(float));

			sleep(a);    //wait for random time interval
		}
		
		close(fd[WRITE_END]);
	}

	else {    //parent process
		//close the write end since we're the consumer
		close(fd[WRITE_END]);

		float ans;
		for (int i=0; i<n; i++) {
			read(fd[READ_END], &ans, sizeof(float)); 
			printf("z[%d]=%lf\n", i, ans);
		}
		
		wait(NULL);
		close(fd[READ_END]);

		printf("Child Complete\n");
	}		

	return 0;
}
