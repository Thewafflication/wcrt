/**
 * @file stdio_threads.c
 * @brief Verifies concurrent stream allocation for TC-0074.
 */

#define WCRT_POSIX 1

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define WORKERS 4
#define ROUNDS 64
#define PATTERN 64

static pthread_mutex_t gate_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gate_condition = PTHREAD_COND_INITIALIZER;
static int gate_waiting;
static unsigned long gate_generation;

/** @brief Stream each worker holds at the current rendezvous. */
static FILE *opened[WORKERS];
/** @brief Descriptor each worker holds at the current rendezvous. */
static int descriptors[WORKERS];
/** @brief First failure code observed by each worker, or zero. */
static int failures[WORKERS];
/** @brief Identity passed to each worker. */
static int identities[WORKERS];

/**
 * @brief Blocks until every worker has reached the same rendezvous.
 *
 * Each worker executes the same number of rendezvous per round on every
 * path, including failure paths, so the barrier cannot deadlock.
 */
static void gate_wait(void)
{
    unsigned long generation;

    pthread_mutex_lock(&gate_mutex);
    generation = gate_generation;
    ++gate_waiting;
    if (gate_waiting == WORKERS) {
        gate_waiting = 0;
        ++gate_generation;
        pthread_cond_broadcast(&gate_condition);
    } else {
        while (generation == gate_generation) {
            pthread_cond_wait(&gate_condition, &gate_mutex);
        }
    }
    pthread_mutex_unlock(&gate_mutex);
}

/**
 * @brief Records a worker failure, keeping the first code observed.
 *
 * @param identity Worker index.
 * @param code Nonzero failure code.
 */
static void record_failure(int identity, int code)
{
    if (failures[identity] == 0) {
        failures[identity] = code;
    }
}

/**
 * @brief Confirms every worker holds a distinct, usable stream.
 *
 * Called by one worker while all workers hold an open stream, so an aliased
 * table slot is observable rather than timing dependent.
 */
static void check_distinct(void)
{
    int left;
    int right;

    for (left = 0; left < WORKERS; ++left) {
        if (descriptors[left] < 3) {
            record_failure(0, 7);
        }
        for (right = left + 1; right < WORKERS; ++right) {
            if (opened[left] == opened[right]) {
                record_failure(0, 8);
            }
            if (descriptors[left] == descriptors[right]) {
                record_failure(0, 9);
            }
        }
    }
}

/**
 * @brief Opens, writes, verifies, and closes one stream per round.
 *
 * @param argument Pointer to this worker's identity.
 * @return A null pointer.
 */
static void *worker(void *argument)
{
    int identity = *(int *)argument;
    char path[32];
    char written[PATTERN];
    char observed[PATTERN];
    FILE *stream;
    int round;

    memset(written, 'a' + identity, sizeof(written));
    sprintf(path, "tc0074-%d.tmp", identity);
    for (round = 0; round < ROUNDS; ++round) {
        stream = fopen(path, "w+b");
        opened[identity] = stream;
        descriptors[identity] = stream == NULL ? -1 : fileno(stream);
        gate_wait();
        if (identity == 0) {
            check_distinct();
        }
        gate_wait();
        if (stream == NULL) {
            record_failure(identity, 1);
        } else {
            if (fwrite(written, 1, PATTERN, stream) != PATTERN) {
                record_failure(identity, 2);
            }
            if (fseek(stream, 0, SEEK_SET) != 0) {
                record_failure(identity, 3);
            }
            memset(observed, 0, sizeof(observed));
            if (fread(observed, 1, PATTERN, stream) != PATTERN) {
                record_failure(identity, 4);
            }
            if (memcmp(written, observed, PATTERN) != 0) {
                record_failure(identity, 5);
            }
        }
        gate_wait();
        if (stream != NULL && fclose(stream) != 0) {
            record_failure(identity, 6);
        }
        remove(path);
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[WORKERS];
    int index;

    for (index = 0; index < WORKERS; ++index) {
        identities[index] = index;
        if (pthread_create(&threads[index], NULL, worker,
            &identities[index]) != 0) {
            return 1;
        }
    }
    for (index = 0; index < WORKERS; ++index) {
        if (pthread_join(threads[index], NULL) != 0) {
            return 2;
        }
    }
    for (index = 0; index < WORKERS; ++index) {
        if (failures[index] != 0) {
            return 16 + index * 16 + failures[index];
        }
    }
    pthread_cond_destroy(&gate_condition);
    pthread_mutex_destroy(&gate_mutex);
    return 0;
}
