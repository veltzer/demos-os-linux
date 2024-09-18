# Exercise 3

Write a program that will update a single variable called LockedCounter.
We have to protect the variable so that only one thread will be able to update it.

We will use 3 threads.
* The initial value of LockedCounter is zero.
* Use 3 threads each one of them will try to increment the LockedCounter.
* Each thread will use mutex to prevent other threads from updating or testing
    this variable at the same time.
* Only if LockedCounter modulus 3 = n (for thread n) then thread n will do
    the increment.
* Each thread should make X attempts (constant) to change the LockedCounter.
* At the end print the value of LockedCounter as well as the total number of
    attempts for changing the counter.
* What do you deduce from the results of this exercise? (the difference in values between the LockedCounter and the attempts...).
* Answer the following questions:
    * Are the results good?
    * What is the influence of printing inside the loop?
    * Would yielding help?
    * How would you improve it further?
    * How would the thread number influence the result?
    * What will happen if you increase number of cores?

Notes:
use slide 3-39 for mutex support for this exercise.

## Solution notes

Are the results good?
* Nope. We attempt many times per each time slice.
What is the influence of printing inside the loop?
* Improves the results because print is read which yields the CPU.
Would yielding help?
* Yes, if the two threads in question are on the same CPU.
How would you improve it further?
* A condition could help by waking the right thread each time.
How would the thread number influence the result?
* Increasing the thread number would cause more problems since the yield would usually
be wrong. Reducing the thread number would cause less problems.
* What will happen if you increase number of cores?
Usually will cause more problems because more attempts will be made in parallel and will fail.
(decreasing the number of cores will be better...).
