# Signals

Write an application that uses temporary files in `/tmp` and simulates database operations.
* Stamp your temporary files with your PID extension.
* Block critical sections of database update from signals.
* Create a signal handler that will perform cleanup on exit.
    * Finish database update and close.
    * Remove temporary files.
* ignore CTRL-c input, only exit when kill is sent and then exit nicely.
