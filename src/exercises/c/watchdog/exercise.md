Write a watchdog type process.
Use multiprocessing (fork/exec/wait).
Use signal handling to get signaled when a child dies.
If a child died from abnormal causes (WIFSIGNLED) then respawn the
child (fork).
