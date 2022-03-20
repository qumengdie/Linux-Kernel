#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

#define NUM_TRAILS 100000

float nhits;
sem_t sem;
void *WorkerThread(void *pvar);

int main(int argc, char *argv[])
{
    pthread_t workers[4]; /* the thread identifier */
    pthread_attr_t attr; /* set of thread attributes */
    int s;

    /* Initialize a thread-shared mutex with the value 1 */
    if (sem_init(&sem, 0, 1) == -1) {
        fprintf(stderr, "sem_init failed");
		return -1;
    }

    /* set the default attributes of the thread */
    pthread_attr_init(&attr);

    /* create the threads */
    for (int i=0; i<4; i++) {
        s = pthread_create(&workers[i], &attr, WorkerThread, argv[1]);
        if (s != 0) {
            fprintf(stderr, "pthread_create failed");
            return -1;
        }
    }
    
    /* wait for the threads to exit */
    for (int i=0; i<4; i++) {
        s = pthread_join(workers[i], NULL);
        if (s != 0) {
            fprintf(stderr, "pthread_join failed");
            return -1;
        }
    }
    
    float prob = nhits / (4*NUM_TRAILS) * 100;

    //printf("nhits = %lf\n", nhits);
    printf("Probability = %lf%%\n", prob);

    return 0;
}

/* The thread will execute in this function */
void *WorkerThread(void *pvar)
{
    int n = atoi(pvar);
    int arr[n];

    unsigned int rand_state = (unsigned int) time(NULL) + pthread_self(); 
    
    int cnt = 0;
    while(cnt < NUM_TRAILS){
        for (int i=0; i<n; i++){
            int day = rand_r(&rand_state) % 365;
            arr[i] = day;
        }
        
        sem_wait(&sem) == -1;

        for (int i=0; i<n; i++) {
            for (int j=i+1; j<n; j++) {
                if (arr[i] == arr[j]) {
                    nhits += 1;
                    goto end;
                }
            }
        }
        end:
        cnt += 1;

        sem_post(&sem) == -1;
    }
    
    pthread_exit(0);
}