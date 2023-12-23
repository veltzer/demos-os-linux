Basic I/O exercise.
1. In directory /proc resides directories that their names are process Ids. The ownership of each directory is the owner of the process. In each directory there is a sub-directory named “fd” that represents the file descriptor table. The content of /proc/xxx/fd are symbolic link files the process has now open.
1. Write a program that scans /proc and finds processes that belong to you.
2. Check which files each of your processes uses.
3. add a switch –r that will only display open files. Verify by running tail -f /var/log/messages
