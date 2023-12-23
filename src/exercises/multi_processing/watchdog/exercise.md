# Watchdog

* Create a watchdog daemon that will start two other daemons that will fake a service.
* Each child daemon will fake a service.
* Each child process will send life sigh signal (use `SIGUSR1`/`SIGUSR2`).
* The watchdog service will make sure it's children are running by expecting a life sign on a time frame window.
* Once any of the child processes dies or not responding, the watchdog process will restart it.
