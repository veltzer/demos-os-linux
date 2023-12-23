Advanced I/O exercise.
1. Write a service that simulates printing. The service will receive print requests. The user will execute a client application to submit print request "mylpr". Each client activation will write to a common "Index" file residing in a common directory it's print request.

	1. Use fixed width record containing a job ID – numeric and a file name null terminated string up to MAXPATHLEN.
	2. Read the index file to find the last print request numeric id. Make sure no one can touch the file at that moment.
	3. Write the new request to the file having an id greater than the last one found.

2. Write the server side of that print simulation. Focus only on the index file. Do not implement actual printing.
	1. Scan the index file and find a job to execute.
	2. Mark the job selected as “Printed” a simple method can be simply by setting the job ID to 0.
	3. BroadcastClient.c, In the current implementation, after the last server replies, the client is blocked on the accept() system call. Improve the client so it will be able to send more requests.
	4. further improve your IRC software to incorporate everything you’ve learned in the course.
