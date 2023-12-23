# Client Server

This is an advanced I/O exercise

* Write a service that simulates printing. The service will receive print requests. The user will execute a client application to submit print request "mylpr". Each client activation will write to a common "Index" file residing in a common directory it's print request.
    * Use fixed width record containing a job ID – numeric and a file name null terminated string up to MAXPATHLEN.
    * Read the index file to find the last print request numeric id. Make sure no one can touch the file at that moment.
    * Write the new request to the file having an id greater than the last one found.
* Write the server side of that print simulation. Focus only on the index file. Do not implement actual printing.
    * Scan the index file and find a job to execute.
    * Mark the job selected as “Printed” a simple method can be simply by setting the job ID to 0.
    * BroadcastClient.c, In the current implementation, after the last server replies, the client is blocked on the accept() system call. Improve the client so it will be able to send more requests.
    * further improve your IRC software to incorporate everything you’ve learned in the course.
