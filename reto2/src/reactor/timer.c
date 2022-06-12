#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "timer.h"

/*
 * Utility functions
 */
/* res = a - b */
void timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	if (res->tv_usec < 0)
	{
		--res->tv_sec;
		res->tv_usec += 1000000;
	}
}

/* res = a + b */
void timeval_add(struct timeval *res, struct timeval *a, struct timeval *b)
{
	res->tv_sec = a->tv_sec + b->tv_sec + a->tv_usec / 1000000 + b->tv_usec / 1000000;
	res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}

/* a < b */
int timeval_less(struct timeval *a, struct timeval *b)
{
	return (a->tv_sec < b->tv_sec) ||
		   ((a->tv_sec == b->tv_sec) && (a->tv_usec < b->tv_usec));
}

/* wait until next_activation (absolute time) */
void delay_until(struct timeval *next_activation)
{
	struct timeval now, timeout;
	gettimeofday(&now, NULL);
	timeval_sub(&timeout, next_activation, &now);
	select(0, NULL, NULL, NULL, &timeout);
}