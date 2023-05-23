This example shows that 'printf' is actually a macro in current glibc versions.

You can see this by analyzing the binary creates and seeing that a call to 'printf'
is nowhere to be found and is replaced by a call to 'puts'.
