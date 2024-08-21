#include <sched.h>
#include <assert.h>

int main() {
	// first set the scheduling policy of this app to SCHED_FIFO with priority 99
	struct sched_param s;
	s.sched_priority=99;
	int ret=sched_setscheduler(0, SCHED_FIFO, &s);
	assert(ret!=-1);
	while(1) {
	}
	return 0;
}
