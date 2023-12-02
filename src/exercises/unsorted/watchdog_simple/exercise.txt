Write a watchdog type double process.

The watchdog is the parent
The interesting process is the child

The parent will restrart the child whenever the child
dies of *abnormal circumstances*.

If the child dies of normal circumstances, the watchdog will
terminate as well.

Use:
	fork
	wait
