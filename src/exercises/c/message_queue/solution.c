#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct _pthread_mq_t {
	int size;
	int* data;
	int min;
	int max;
	pthread_mutex_t m;
	pthread_cond_t cond_empty;
	pthread_cond_t cond_full;
} pthread_mq_t;

int pthread_mq_init(pthread_mq_t* p, int size) {
	int res;
	p->size=size;
	p->data=(int*)malloc(sizeof(int)*size);	
	if(p->data==NULL) return 1;
	p->min=0;
	p->max=0;
	res=pthread_mutex_init(&(p->m), NULL);
	if(res!=0) return res;
	res=pthread_cond_init(&(p->cond_empty), NULL);
	if(res!=0) return res;
	res=pthread_cond_init(&(p->cond_full), NULL);
	if(res!=0) return res;
	return 0;
}

int pthread_mq_get(pthread_mq_t* mq, int* value) {
	int res;
	res = pthread_mutex_lock(&(mq->m));
	if(res!=0) return res;
	// while the queue is full go to sleep
	while(mq->max==mq->min) {
		res = pthread_cond_wait(&(mq->cond_empty), &(mq->m));
		if(res!=0) return res;
	}
	*value=mq->data[mq->min];
	// if we turned the queue from full -> non full, wake up whoever is waiting
	if((mq->max+1)%mq->size==mq->min) {
		res = pthread_cond_broadcast(&(mq->cond_empty));
		if(res!=0) return res;
	}
	mq->min++;
	mq->min%=mq->size;
	res = pthread_mutex_unlock(&(mq->m));
	if(res!=0) return res;
	return 0;
}

int pthread_mq_put(pthread_mq_t* mq, int value) {
	int res;
	res = pthread_mutex_lock(&(mq->m));
	if(res!=0) return res;
	// while the queue is full go to sleep
	while((mq->max+1)%mq->size==mq->min) {
		res = pthread_cond_wait(&(mq->cond_full), &(mq->m));
		if(res!=0) return res;
	}
	mq->data[mq->max]=value;
	// if we turned the queue from empty -> non empty, wake up whoever is waiting
	if(mq->max==mq->min) {
		res = pthread_cond_broadcast(&(mq->cond_empty));
		if(res!=0) return res;
	}
	mq->max++;
	mq->max%=mq->size;
	res = pthread_mutex_unlock(&(mq->m));
	if(res!=0) return res;
	return 0;
}

int pthread_mq_destroy(pthread_mq_t* p) {
	int res;
	free(p->data);
	res = pthread_mutex_destroy(&(p->m));
	if(res!=0) return res;
	res = pthread_cond_destroy(&(p->cond_empty));
	if(res!=0) return res;
	res = pthread_cond_destroy(&(p->cond_full));
	if(res!=0) return res;
	return 0;
}

void* func_producer(void* arg) {
	int res;
	pthread_mq_t* mq=(pthread_mq_t*)arg;
	while(1) {
		int value;
		printf("give me a number: ");
		fflush(stdout);
		res=scanf("%d", &value);
		assert(res==1);
		res = pthread_mq_put(mq, value);
		assert(res==0);
		if(value==666) {
			break;
		}
	}
	return NULL;
}

void* func_consumer(void* arg) {
	int res;
	pthread_mq_t* mq=(pthread_mq_t*)arg;
	while(1) {
		int value;
		res = pthread_mq_get(mq, &value);
		assert(res==0);
		printf("consumer: got value %d\n", value);
		if(value==666) {
			break;
		}
	}
	return NULL;
}

int main() {
	int res;
	pthread_t consumer, producer;
	pthread_mq_t mq;
	res=pthread_mq_init(&mq, 10);
	if(res!=0) return res;
	res=pthread_create(&producer, NULL, func_producer, &mq);
	if(res!=0) return res;
	res=pthread_create(&consumer, NULL, func_consumer, &mq);
	if(res!=0) return res;
	res=pthread_join(producer, NULL);
	if(res!=0) return res;
	res=pthread_join(consumer, NULL);
	if(res!=0) return res;
	res=pthread_mq_destroy(&mq);
	if(res!=0) return res;
	return EXIT_SUCCESS;
}
