/*
 * Classic CPU memory reordering demonstration (Preshing / Dekker style).
 *
 * Two threads each write one variable then read the other's variable.
 * Without memory barriers, the CPU may reorder the write and read within
 * each thread.  That makes it possible for BOTH threads to read 0, even
 * though at least one write must have happened before any read -- an
 * "impossible" outcome under sequential consistency.
 *
 * Thread 1:  x = 1;  r1 = y;
 * Thread 2:  y = 1;  r2 = x;
 *
 * If memory is sequentially consistent, at least one thread sees the
 * other's write, so (r1==0 && r2==0) should never happen.
 * With reordering it does happen, and we count how many times.
 *
 * Compile WITHOUT barriers: g++ -O2 -pthread ...       (reordering visible)
 * Compile WITH    barriers: g++ -O2 -pthread -DUSE_BARRIERS  (never reorders)
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// ---------- shared state ------------------------------------------------

static int x, y;
static int r1, r2;

static sem_t begin1, begin2, end1, end2;

// ---------- barrier helper ----------------------------------------------

static inline void store_load_barrier(void) {
#ifdef USE_BARRIERS
    __asm__ volatile("mfence" ::: "memory");
#else
    __asm__ volatile("" ::: "memory");  // compiler barrier only, no CPU fence
#endif
}

// ---------- threads -----------------------------------------------------

static void *thread1(void * /*arg*/) {
    for (;;) {
        sem_wait(&begin1);
        x = 1;
        store_load_barrier();
        r1 = y;
        sem_post(&end1);
    }
    return nullptr;
}

static void *thread2(void * /*arg*/) {
    for (;;) {
        sem_wait(&begin2);
        y = 1;
        store_load_barrier();
        r2 = x;
        sem_post(&end2);
    }
    return nullptr;
}

// ---------- main --------------------------------------------------------

int main(int argc, char *argv[]) {
    long iterations = 1000000;
    if (argc > 1)
        iterations = atol(argv[1]);

    sem_init(&begin1, 0, 0);
    sem_init(&begin2, 0, 0);
    sem_init(&end1,   0, 0);
    sem_init(&end2,   0, 0);

    pthread_t t1, t2;
    pthread_create(&t1, nullptr, thread1, nullptr);
    pthread_create(&t2, nullptr, thread2, nullptr);

    long reorders = 0;

    for (long i = 0; i < iterations; ++i) {
        x = 0; y = 0;

        // Release both threads simultaneously.
        sem_post(&begin1);
        sem_post(&begin2);

        // Wait for both to finish.
        sem_wait(&end1);
        sem_wait(&end2);

        // The "impossible" outcome: both reads saw 0.
        if (r1 == 0 && r2 == 0)
            ++reorders;
    }

    printf("Iterations : %ld\n", iterations);
    printf("Reorderings: %ld\n", reorders);
    printf("Rate       : %.4f%%\n", 100.0 * reorders / iterations);
    return 0;
}
