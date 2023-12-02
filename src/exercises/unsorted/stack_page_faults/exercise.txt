Preventing page faults on stack exercise
========================================
1. Create an application which creates page fault on the stack
(first use little stack and then little by little use more stack).
Think: How do you allocate more space on the stack each time?
Use either recusion or a data structure allocated directly on the stack.
2. Prevent those page faults in at least two different ways: by
pre-allocating the pages or by calling mlock(2).

* monitor your application using:
	watch ps -C [your app name] -o min_flt
