# `ls`

Write your own implementation of ls -l. Try to make it emit output
which is as close to the ls -l output as possible.
Here is how to compare your output with the output of ls:

```bash
$ cd [somedir] # go to some directory`
$ unalias ls # make ls not do colors (it's not in the exercise)`
$ ls -l > realls # save the output of ls`
$ ~/myls > myls # save the output from my ls
$ diff realls myls # shows the differeces
....
```

You should show: type of file, permissions, hard link count,
owner(in string form), group(in string form), file size, modification
date, and file name. For symbolic links: name -> orig should be displayed.

Write a `Makefile` that compiles your program.
`$ make # should build the program`
`$ make clean # should remove all generated files`

Compile your program with `-Wall -Werror`. This means you do not allow any errors.
Always take care of errors when calling system calls or library calls.

Hints:
Use `stat(2)`, `readlink(2)`, `opendir(3)`, `readdir(3)`, `closedir(3)` , `getpwnam(3)`, `getgrnam(3)`.

Good luck!
