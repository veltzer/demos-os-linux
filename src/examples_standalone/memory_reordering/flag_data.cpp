/*
 * Memory reordering: ready flag + data pattern.
 *
 * Writer sets data=42, then sets ready=1.
 * Reader spins on ready, then reads data.
 *
 * Without a store-load barrier on the writer side, the CPU may reorder
 * the two stores so ready becomes visible before data.  The reader then
 * exits the spin loop but sees data==0 instead of 42.
 *
 * We use a semaphore to reset both threads each iteration and count how
 * many times the reader observes the "impossible" stale value.
 *
 * WHY YOU WILL LIKELY SEE ZERO REORDERINGS ON x86-64:
 *
 * Two reasons conspire to hide the effect:
 *
 * 1. x86 TSO only permits store-LOAD reordering (a store may be delayed in
 *    the store buffer while a subsequent load to a different address goes
 *    ahead).  It does NOT permit store-STORE reordering.  The writer does
 *    two stores (data then ready); on x86 these are always visible to other
 *    cores in program order.  So once the reader sees ready==1, data==42 is
 *    already guaranteed to be visible too.
 *
 * 2. The semaphore calls go through the kernel, which issues full memory
 *    barriers internally.  This closes any remaining window between the
 *    writer's sem_post and the reader's sem_wait.
 *
 * On a weakly-ordered architecture (ARM, POWER) store-store reordering
 * IS allowed, and stale reads would be observed without the mfence.
 * The Dekker-style test in main.cpp demonstrates a reordering that x86
 * DOES permit (store-load) and is therefore a better vehicle on this arch.
 *
 * Compile WITHOUT barriers: g++ -O2 -pthread ...
 * Compile WITH    barriers: g++ -O2 -pthread -DUSE_BARRIERS
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

static int data;
static int ready;
static int observed;   // what the reader actually saw in data

static sem_t begin_writer, begin_reader, end_writer, end_reader;

static inline void store_barrier(void) {
#ifdef USE_BARRIERS
    __asm__ volatile("mfence" ::: "memory");
#else
    __asm__ volatile("" ::: "memory");  // compiler barrier only
#endif
}

static void *writer(void * /*arg*/) {
    for (;;) {
        sem_wait(&begin_writer);
        data = 42;
        store_barrier();
        ready = 1;
        sem_post(&end_writer);
    }
    return nullptr;
}

static void *reader(void * /*arg*/) {
    for (;;) {
        sem_wait(&begin_reader);
        while (!ready)
            __asm__ volatile("" ::: "memory");  // re-load ready each iteration

        observed = data;
        sem_post(&end_reader);
    }
    return nullptr;
}

int main(int argc, char *argv[]) {
    long iterations = 1000000;
    if (argc > 1)
        iterations = atol(argv[1]);

    sem_init(&begin_writer, 0, 0);
    sem_init(&begin_reader, 0, 0);
    sem_init(&end_writer,   0, 0);
    sem_init(&end_reader,   0, 0);

    pthread_t tw, tr;
    pthread_create(&tw, nullptr, writer, nullptr);
    pthread_create(&tr, nullptr, reader, nullptr);

    long stale = 0;

    for (long i = 0; i < iterations; ++i) {
        data  = 0;
        ready = 0;

        sem_post(&begin_writer);
        sem_post(&begin_reader);

        sem_wait(&end_writer);
        sem_wait(&end_reader);

        if (observed != 42)
            ++stale;
    }

    printf("Iterations   : %ld\n", iterations);
    printf("Stale reads  : %ld\n", stale);
    printf("Rate         : %.4f%%\n", 100.0 * stale / iterations);
    return 0;
}
