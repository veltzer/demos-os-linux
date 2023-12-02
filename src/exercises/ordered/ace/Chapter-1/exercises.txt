Exercise 0
Write the ACE hello world program according to the slides and verify that it works correctly.
Check what libraries are linked in to your ACE application on the specific operating system
you are running on. (Windows: use binary inspector, linux/unix: use ldd).

Where do ACE_DEUBG and ACE_ERROR messages go to ?

Exercise 1
1. Write a program that will have two functions FunA and FunB.
	use a loop counter to call Twice FunA and this function will call FunB.
2. Each function will print a message when entered, using the ACE utility.
3. Use the ACE to set and unset this capability.
4. In each function print debugging information using 3 levels:
	1. LM_TRACE
	2. LM_DEBUG
	3. LM_INFO
	Use the following options in each output line:
	1. Print program pid.
	2. Print line number.
	3. Print File Name.

Try to verify what causes indentation.

Hint: Controlling the output may be done by the following command:
ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);
