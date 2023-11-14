#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX_SEATS 30
struct Seat {
    int economy;
    int business;
    int first;
};
struct Seat seats = {10, 10, 10};
sem_t mutex, wrt;
int readerCount = 0;
void *passenger(void *arg);
void *agent(void *arg);
struct ThreadParams {
    int id;
};
int main() {
    int numPassengers = MAX_SEATS; 
    int numAgents = 2;
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);
    pthread_t passengerThreads[numPassengers], agentThreads[numAgents];
    struct ThreadParams passengerParams[numPassengers];
    for (int i = 0; i < numPassengers; i++) {
        passengerParams[i].id = i + 1; 
        pthread_create(&passengerThreads[i], NULL, passenger, &passengerParams[i]);
    }
struct ThreadParams agentParams[numAgents];
    for (int i = 0; i < numAgents; i++) {
        agentParams[i].id = i + 1;
        pthread_create(&agentThreads[i], NULL, agent, &agentParams[i]);
    }
    for (int i = 0; i < numPassengers; i++) {
        pthread_join(passengerThreads[i], NULL);
    }
    for (int i = 0; i < numAgents; i++) {
        pthread_join(agentThreads[i], NULL);
    }
    sem_destroy(&mutex);
    sem_destroy(&wrt);
    return 0;
}
void *passenger(void *arg) {
    struct ThreadParams *params = (struct ThreadParams *)arg;
 int passengerId = params->id;
    for (int i = 0; i < 5; i++) {
        sem_wait(&mutex);
        readerCount++;
        if (readerCount == 1) {
            sem_wait(&wrt);
        }
        sem_post(&mutex);
        printf("Passenger %d checking availability: Economy=%d, Business=%d, First=%d\n",
               passengerId, seats.economy, seats.business, seats.first);

        sem_wait(&mutex);
        readerCount--;
        if (readerCount == 0) {
            sem_post(&wrt);
        }
        sem_post(&mutex);
        usleep(100000);
    }
    pthread_exit(NULL);
}
void *agent(void *arg) {
    struct ThreadParams *params = (struct ThreadParams *)arg;
    int agentId = params->id;

    for (int i = 0; i < 5; i++) {
        sem_wait(&wrt);
        seats.economy--;
        seats.business--;
        seats.first--;
        printf("Agent %d updated availability: Economy=%d, Business=%d, First=%d\n",
               agentId, seats.economy, seats.business, seats.first);

        sem_post(&wrt);
        usleep(200000);
    }
    pthread_exit(NULL);
}

