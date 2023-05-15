#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ctype.h>

#define TAILLE_TAMPON 5
#define NB_PRODUCTEURS 2
#define NB_CONSOMMATEURS 2

sem_t *scons, *sprod;
volatile char buffer[TAILLE_TAMPON];
char c = 'a';

char Produire(int ip) {
    buffer[ip] = c;
    return c;
}

void *Producteur(void *args) {
    int ip = *(int *)args;
    while (1) {
        sem_wait(&sprod[ip]);

        // Produit un caractère
        char carac = Produire(ip);
        printf("Production du producteur %d à l'index %d : %c\n", ip, ip, carac);

        // Incrémente le caractère suivant
        c++;

        // Si on atteint le caractère 'z', revenir au caractère 'a'
        if (c > 'z')
            c = 'a';

        sem_post(&scons[ip]);

        sleep(1);
    }
}

char Consommer(int ic) {
    char carac = buffer[ic];

    // Met le caractère en majuscule
    char Maj_c = toupper(carac);

    return Maj_c;
}

void *Consommateur(void *args) {
    int ic = *(int *)args;
    while (1) {
        sem_wait(&scons[ic]);

        // Consomme
        char Maj_c = Consommer(ic);

        printf("Consommation du consommateur %d à l'index %d : %c\n", ic, ic, Maj_c);

        sem_post(&sprod[ic]);

        sleep(1);
    }
}

int main() {
    pthread_t prod_th[NB_PRODUCTEURS], cons_th[NB_CONSOMMATEURS];

    int indices_producteurs[NB_PRODUCTEURS];
    int indices_consommateurs[NB_CONSOMMATEURS];

    scons = sem_open("/scons", O_CREAT, 0644, 0);
    sprod = sem_open("/sprod", O_CREAT, 0644, TAILLE_TAMPON);

    for (int i = 0; i < NB_PRODUCTEURS; i++) {
        indices_producteurs[i] = i;
        pthread_create(&prod_th[i], NULL, Producteur, &indices_producteurs[i]);
    }

    for (int i = 0; i < NB_CONSOMMATEURS; i++) {
        indices_consommateurs[i] = i;
        pthread_create(&cons_th[i], NULL, Consommateur, &indices_consommateurs[i]);
    }

    for (int i = 0; i < NB_PRODUCTEURS; i++) {
        pthread_join(prod_th[i], NULL);
    }

    for (int i = 0; i < NB_CONSOMMATEURS; i++) {
        pthread_join(cons_th[i], NULL);
    }

    sem_close(scons);
    sem_close(sprod);

    return 0;
}
