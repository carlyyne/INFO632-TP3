#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

    /* Global variables */
    int x = 0;
    sem_t *sem_sync1,*sem_sync2;
    void a1();
    void a2();

    void b1();
    void b2();

    /* Thread function */
    void *p1(void *arg)
    {
        a1();
        sem_post(sem_sync1); //V
        sem_wait(sem_sync2); //P
        a2();
    }

    void *p2(void *arg)
    {
        sem_wait(sem_sync1); //P
        b2();
        b1();
        sem_post(sem_sync2); //V
    }


    void a1(){
        sleep(2);
        printf("Execution de la tache a1\n");
    }

    void a2(){
        sleep(2);
        printf("Execution de la tache a2\n");
    }

    void b1(){
        printf("Execution de la tache b1\n");
    }

    void b2(){
        printf("Execution de la tache b2\n");
    }

    int main () {
        pthread_t thread1, thread2;
        sem_sync1 = sem_open("/sem_sync1",O_CREAT,0644,0);
        sem_sync2 = sem_open("/sem_sync2",O_CREAT,0644,0);

        /* semaphore sync should be initialized by 0 */
        if (sem_sync1 == SEM_FAILED || sem_sync2 == SEM_FAILED) {
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
        sem_close(sem_sync2);
        exit(0);
    }
