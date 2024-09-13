Write a /dev/zero like device module.
This device only supports the 'read'
operation. Whenever it is called it fills
the user buffer with zeros.

You will to select a major number for your module (say 190).
You will need the function called '__clear_user'
that can se a user space buffer to all zeros.
