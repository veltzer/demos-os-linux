THIS IS A SIMPLE EXERCISE: DO NOT DO SLEEPING OR LOCKING AT FIRST STAGE...

This is a pipe exercise:

create a module that exposes files in /dev of the form:
/dev/pipe[0-8)

Each pipe represents a PAGE_SIZE buffer in kernel.
Allow reading and writing to the pipe.

Take care of synchronization.

If a reader wants to read and there is no data in the pipe and also there
are no writers, return EOF to the reader.

If, on the other hand, a reader wants to read data and there IS a writer connected
to the pipe but no data in the pipe, put the reader to sleep until the writer
decides to write something or disconnect from the pipe.

If the writer writes data, wakeup the reader and consume the data.
If the writer disconnects from the pipe, wakeup the reader and return EOF.

Compare your results with the Linux implementation of a pipe, use pipemeter.
Are your results better? Worse? Why?
