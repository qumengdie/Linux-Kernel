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

#define BUF_LEN 10

int main()
{
	pid_t pid;
	pid_t ret;
	int status;
	int n;
	printf("Please input the length n: ");
	scanf("%d", &n);

	// the size of shm object	
	int size = BUF_LEN*sizeof(int) + 2*sizeof(int);
	const char *name = "arr";
	int fd;    //shm file descriptor
	float *ptr;    //pointer to shm object
	
	// create the shm object
	fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	// configure the size of shm object
	ftruncate(fd, size);
	// memeory map the shared memory object
	ptr = (float *)
	 mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	int* pidx=(int*) (&ptr[BUF_LEN]);
	ptr[BUF_LEN] = 0;
	ptr[BUF_LEN+1] = 0;
	#define IN pidx[0]
	#define OUT pidx[1]
	
	// create child process
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Fork Failed");
		return 1;
	}

	else if (pid == 0) {    //child process
		double a;
		srand((unsigned)time(NULL));

		float next_produced;
		for (int i=0; i<n; i++){
			a = (double)rand() / RAND_MAX * 4.999;
			next_produced = pow(0.5 * i, 2);

			while (((IN+1) % (BUF_LEN-1)) == OUT); 

			ptr[IN] = next_produced;
			sleep(a);
			IN = (IN+1) % (BUF_LEN-1);
		}
		_exit(0);
	}

	else {    //parent process
		float next_consumed;
		int cnt = 0;
		while (!(ret = waitpid(pid, &status, WNOHANG))) {
			if (cnt==n) break;
			while (IN == OUT); 
			next_consumed = ptr[OUT];
			printf("z[%d] = %lf\n", cnt, next_consumed);
			fflush(stdout);
			OUT = (OUT + 1) % (BUF_LEN-1);
			cnt++;
		}
		printf("Child Complete\n");
	}		
	shm_unlink(name);

	return 0;
}
