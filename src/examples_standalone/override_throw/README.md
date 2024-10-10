# Override the throw mechanism

This example shows how to override libstdc++'s internal exception throwing
mechanism in order that every thrown exception will show you a stack trace.

The idea is to override `__cxw_throw` correctly.

References:
[here](https://gist.github.com/nkuln/2020860)
