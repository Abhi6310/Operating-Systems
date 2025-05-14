#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define NUM_PRODUCERS 4
#define NUM_CONSUMERS 4
#define ITEMS_PER_PRODUCER 20
#define POISON_PILL "DONE"  // Special string to signal consumers to exit

// Global shared array instance
array shared_array;

// Producer thread function.
// Each producer will generate ITEMS_PER_PRODUCER hostnames and add them to the shared array.
void* producer(void* arg) {
    int id = *(int *)arg;
    char hostname[MAX_NAME_LENGTH];
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        // Format a hostname string unique to this producer and iteration.
        snprintf(hostname, MAX_NAME_LENGTH, "hostname-%d-%d", id, i);
        if (array_put(&shared_array, hostname) != 0) {
            fprintf(stderr, "Producer %d: Failed to put item\n", id);
        } else {
            printf("Producer %d: put %s\n", id, hostname);
        }
        // Optional: sleep to simulate work and force thread interleaving.
        usleep(10000); // 10 ms
    }
    return NULL;
}

// Consumer thread function.
// Each consumer continuously gets items from the shared array and exits upon receiving the poison pill.
void* consumer(void* arg) {
    int id = *(int *)arg;
    while (1) {
        char *item = NULL;
        if (array_get(&shared_array, &item) != 0) {
            fprintf(stderr, "Consumer %d: Failed to get item\n", id);
        } else {
            // Check for the poison pill.
            if (strcmp(item, POISON_PILL) == 0) {
                free(item);
                printf("Consumer %d: Received poison pill, exiting.\n", id);
                break;
            }
            printf("Consumer %d: got %s\n", id, item);
            free(item);
        }
        // Optional: sleep to simulate processing time.
        usleep(15000); // 15 ms
    }
    return NULL;
}

int main() {
    pthread_t prod_threads[NUM_PRODUCERS];
    pthread_t cons_threads[NUM_CONSUMERS];
    int prod_ids[NUM_PRODUCERS];
    int cons_ids[NUM_CONSUMERS];

    // Initialize the shared array.
    if (array_init(&shared_array) != 0) {
        fprintf(stderr, "Error initializing shared array\n");
        exit(EXIT_FAILURE);
    }

    // Create consumer threads first.
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cons_ids[i] = i;
        if (pthread_create(&cons_threads[i], NULL, consumer, &cons_ids[i]) != 0) {
            fprintf(stderr, "Error creating consumer thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Create producer threads.
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        prod_ids[i] = i;
        if (pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]) != 0) {
            fprintf(stderr, "Error creating producer thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all producer threads to finish.
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    printf("All producers finished.\n");

    // After producers finish, insert a poison pill for each consumer so they can exit.
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (array_put(&shared_array, POISON_PILL) != 0) {
            fprintf(stderr, "Error putting poison pill\n");
        }
    }

    // Wait for all consumer threads to finish.
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(cons_threads[i], NULL);
    }
    printf("All consumers finished.\n");

    // Clean up the shared array resources.
    array_free(&shared_array);

    return 0;
}
