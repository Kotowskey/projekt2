#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h> // Pseudolosowosc przychodzenia klientow
#include <math.h>

pthread_mutex_t mutex;
pthread_cond_t cond_barber;
pthread_cond_t cond_customer;

int waiting_customers = 0;
bool barber_chair = false;
int num_chairs;

void time_wasting(int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += sqrt(i) + cos(i) + tan(i) + sqrt(10061661000);
    }
}

void* barber(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (waiting_customers == 0) {
            printf("Golibroda śpi...\n");
            pthread_cond_wait(&cond_barber, &mutex);
        }

        printf("Golibroda zaczyna strzyc...\n");
        waiting_customers--;
        barber_chair = true;

        pthread_cond_signal(&cond_customer);
        pthread_mutex_unlock(&mutex);

        // Symulacja strzyżenia
        time_wasting(10000000);

        pthread_mutex_lock(&mutex);
        printf("Golibroda zakończył strzyżenie.\n");
        barber_chair = false;
        pthread_cond_broadcast(&cond_customer);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* customer(void* arg) {
    int id = *(int*)arg;
    pthread_mutex_lock(&mutex);

    if (waiting_customers < num_chairs) {
        printf("Klient %d w poczekalni. Poczekalnia: %d/%d\n", id, waiting_customers, num_chairs);
        waiting_customers++;

        pthread_cond_signal(&cond_barber);

        while (barber_chair) {
            pthread_cond_wait(&cond_customer, &mutex);
        }

        printf("Klient %d jest strzyżony.\n", id);
        barber_chair = true;
    } else {
        printf("Klient %d odchodzi, brak miejsca.\n", id);
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Podaj prawidłową liczbę argumentów!\n");
        return EXIT_FAILURE;
    }

    num_chairs = atoi(argv[1]);
    if (num_chairs <= 0) {
        printf("Liczba krzeseł musi być większa niż 0!\n");
        return EXIT_FAILURE;
    }

    pthread_t barber_thread;
    pthread_t customer_thread;
    int customer_id = 1;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_barber, NULL);
    pthread_cond_init(&cond_customer, NULL);

    pthread_create(&barber_thread, NULL, barber, NULL);

    while (1) {
        pthread_create(&customer_thread, NULL, customer, &customer_id);
        customer_id++;
        sleep(1); // symulacja pojawiania się klientów
    }

    pthread_cancel(barber_thread);
    pthread_join(barber_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_barber);
    pthread_cond_destroy(&cond_customer);

    return EXIT_SUCCESS;
}
