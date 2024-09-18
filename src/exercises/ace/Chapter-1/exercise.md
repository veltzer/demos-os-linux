# Exercise 0

Write the ACE hello world program according to the slides and verify that it works correctly.
Check what libraries are linked in to your ACE application on the specific operating system
you are running on. (Windows: use binary inspector, Linux/UNIX: use ldd).

Where do `ACE_DEUBG` and `ACE_ERROR` messages go to ?

## Exercise 1

* Write a program that will have two functions Function A and Function B.
    use a loop counter to call Twice Function A and this function will call Function B.
* Each function will print a message when entered, using the ACE utility.
* Use the ACE to set and unset this capability.
* In each function print debugging information using 3 levels:
    * `LM_TRACE`
    * `LM_DEBUG`
    * `LM_INFO`
    Use the following options in each output line:
    * Print program pid.
    * Print line number.
    * Print File Name.

Try to verify what causes indentation.

Hint: Controlling the output may be done by the following command:
`ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);`
