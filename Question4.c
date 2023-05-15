#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ctype.h>

#define TAILLE_TAMPON 5

sem_t *scons, *sprod;
volatile char buffer[TAILLE_TAMPON];
char c = 'a';

char Produire(int ip) {
    buffer[ip] = c;
    return c;
}

void *Producteur(void *args) {
    int ip = 0;
    while (1) {

        // Produit un caractère
        char carac = Produire(ip);
        printf("Production à l'index %d : %c\n", ip, carac);

        sem_wait(sprod);

        // Incrémente le caractère suivant
        c++;

        // Si on atteint le caractère 'z', revenir au caractère 'a'
        if (c > 'z')
            c = 'a';

        sem_post(scons);

        ip++;

        // Si on est à la case 5, on revient à la case 0
        if (ip == TAILLE_TAMPON) {
            ip = 0;
        }
    }
}

char Consommer(int ic) {
    char carac = buffer[ic];

    // Met le caractère en majuscule
    char Maj_c = toupper(carac);

    return Maj_c;
}

void *Consommateur(void *args) {
    int ic = 0;
    while (1) {

        sem_wait(scons);

        // Consomme
        char Maj_c = Consommer(ic);

        printf("Consommation à l'index %d : %c\n", ic, Maj_c);

        sem_post(sprod);

        ic++;

        // Si on est à la case 5, on revient à la case 0
        if (ic == TAILLE_TAMPON) {
            ic = 0;
        }

        sleep(1);
    }
}

int main() {
    pthread_t prod_th, cons_th;

    scons = sem_open("/scons", O_CREAT, 0644, 0);
    sprod = sem_open("/sprod", O_CREAT, 0644, TAILLE_TAMPON);

    pthread_create(&prod_th, NULL, Producteur, NULL);
    pthread_create(&cons_th, NULL, Consommateur, NULL);

    pthread_join(prod_th, NULL);
    pthread_join(cons_th, NULL);

    sem_close(scons);
    sem_close(sprod);

    return 0;
}
