#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

    /* Global variables */
    int x = 0;
    sem_t *sem_sync1;

    void a();
    void b();

    /* Thread function */
    void *p1(void *arg)
    {
        a();
        /* send signal to the created thread */
        sem_post(sem_sync1); //V
    }

    void *p2(void *arg)
    {
        /* wait for signal from main thread */
        sem_wait(sem_sync1); //P
        b();
    }

    void a(){
        sleep(10);
        printf("X = %d\n", x);
        x=55;
    }

    void b(){
        printf("X = %d\n", x);
    }

    int main () {
        pthread_t thread1, thread2;
        sem_sync1 = sem_open("/sem_sync1",O_CREAT,0644,0);

        /* semaphore sync should be initialized by 0 */
        if (sem_sync1 == SEM_FAILED) {
            perror("Could not initialize mylock semaphore");
            exit(2);
        }
        if (pthread_create(&thread1, NULL, p1, NULL) < 0) {
            perror("Error: thread cannot be created");
            exit(1);
        }
        if (pthread_create(&thread2, NULL, p2, NULL) < 0) {
            perror("Error: thread cannot be created");
            exit(1);
        }
        /* wait for created thread to terminate */
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

        /* close semaphore sync */
        sem_close(sem_sync1);
        exit(0);
    }
