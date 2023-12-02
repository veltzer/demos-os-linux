Processes exercise
1. Create a watchdog daemon that will start two other daemons that will fake a service.
1. Each child daemon will fake a service.
2. Each child process will send life sigh signal (use SIGUSR1/2).
3. The watchdog service will make sure it's children are running by expecting a life sign on a time frame window.
4. Once any of the child processes dies or not responding, the watchdog process will restart it.
