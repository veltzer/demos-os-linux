# Writing `/dev/zero` like device

This device only supports the `read` operation.
Whenever it is called it fills the user's buffer with zeros.

You will to select a major number for your module (say 190).
You will need the function called `__clear_user`
that can set a user space buffer to all zeros.
