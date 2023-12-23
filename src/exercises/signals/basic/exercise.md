Signals exercise.
Write an application that uses temporary files in /tmp and simulates database operations.
1 Stamp your temporary files with your PID extension.
2 Block critical sections of database update from signals.
3 Create a signal handler that will perform cleanup on exit.
	1 Finish database update and close.
	2 Remove temporary files.
4. ignore ctrl-c input, only exit when kill is sent and then exit nicely.
