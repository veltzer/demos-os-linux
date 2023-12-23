# Fix Threads

In the following code there is a `sleep(3)` function call just before `main()` exits.

```c
void* PrintHello(void *threadid) {
    ...
}

int main(int argc, char** argv, char** envp) {
    const int NUM_THREADS=8;
    pthread_t threads[NUM_THREADS];
    int t[NUM_THREADS];
    ssize_t retval[NUM_THREADS];
    int stime;
    srand(time(NULL));
    for(int i=0; i<NUM_THREADS; i++) {
        t[i]=i;
        CHECK_ZERO_ERRNO(pthread_create(&threads[i], NULL, PrintHello, (void *) &t[i]));
    }
    sleep(40);
    return EXIT_SUCCESS;
}
```

* What will happen if sleep(3) is removed?
* Replace the sleep(3) function with something better.
* Improve your IRC software to use threads.
