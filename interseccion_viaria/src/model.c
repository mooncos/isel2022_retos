#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"
#include "model.h"

/* Timer consts */
struct timeval esperaLarga = {30, 0}; /* 30 segundos */
struct timeval esperaAmbar = {5, 0};  /* 5 segundos */
struct timeval esperaCambio = {1, 0}; /* 1 segundos */

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

/* Estados */
/*
	0 -> Verde principal, rojo secundario
	1 -> Ambar principal, rojo secundario
	2 -> Rojo principal, rojo secundario (estado de cambio de tráfico a secundaria)
	3 -> Rojo principal, verde secundario
	4 -> Rojo principal, ambar secundario
	5 -> Rojo principal, rojo secundario (estado de cambio de tráfico a principal)
*/

/* Transition functions */
static int state0to1(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&z->next, &now);
	return (finEsperaLarga && (*z->e || *z->bp));
}
static int state0to0(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&z->next, &now);
	return (!finEsperaLarga || (!(*z->e) && !(*z->bp)));
}

static int state1to1(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&z->next, &now);
	return (!finEsperaAmbar);
}
static int state1to2(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&z->next, &now);
	return (finEsperaAmbar);
}

static int state2to2(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&z->next, &now);
	return (!finEsperaCambio);
}
static int state2to3(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&z->next, &now);
	return (finEsperaCambio);
}

static int state3to4(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&z->next, &now);
	return (finEsperaLarga || (!(*z->e) && !(*z->bs)));
}
static int state3to3(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&z->next, &now);
	return (!finEsperaLarga && (*z->e || *z->bs));
}

static int state4to4(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&z->next, &now);
	return (!finEsperaAmbar);
}
static int state4to5(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&z->next, &now);
	return (finEsperaAmbar);
}

static int state5to5(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&z->next, &now);
	return (!finEsperaCambio);
}
static int state5to0(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&z->next, &now);
	return (finEsperaCambio);
}

/* Action functions */
static void from0to1(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->e = 0;
	*z->bp = 0;
	*z->sp = AMBAR;
	*z->ss = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&z->next, &now, &esperaAmbar);
	printf(">> From 0 to 1:\n > sP: %d\n > sS: %d\n < e: %d\n < bP: %d\n < bS: %d\n   next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, *z->e, *z->bp, *z->bs, z->next.tv_sec, z->next.tv_usec);
}
static void from0to0(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = VERDE;
	*z->ss = ROJO;
	printf(">> From 0 to 1:\n > sP: %d\n > sS: %d\n < e: %d\n < bP: %d\n < bS: %d\n   next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, *z->e, *z->bp, *z->bs, z->next.tv_sec, z->next.tv_usec);
}

static void from1to1(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = AMBAR;
	*z->ss = ROJO;
	printf(">> From 1 to 1:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}
static void from1to2(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&z->next, &now, &esperaCambio);
	printf(">> From 1 to 2:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}

static void from2to2(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = ROJO;
	printf(">> From 2 to 2:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}
static void from2to3(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = VERDE;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&z->next, &now, &esperaLarga);
	printf(">> From 2 to 3:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}

static void from3to4(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = AMBAR;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&z->next, &now, &esperaAmbar);
	printf(">> From 3 to 4:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}
static void from3to3(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->e = 0;
	*z->bs = 0;
	*z->sp = ROJO;
	*z->ss = VERDE;
	printf(">> From 3 to 3:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}

static void from4to4(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = AMBAR;
	printf(">> From 4 to 4:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}
static void from4to5(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&z->next, &now, &esperaCambio);
	printf(">> From 4 to 5:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}

static void from5to5(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = ROJO;
	*z->ss = ROJO;
	printf(">> From 5 to 5:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}
static void from5to0(fsm_t *this)
{
	fsm_cross_t *z = (fsm_cross_t *)this;
	*z->sp = VERDE;
	*z->ss = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&z->next, &now, &esperaLarga);
	printf(">> From 5 to 0:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", (int)*z->sp, (int)*z->ss, z->next.tv_sec, z->next.tv_usec);
}

/*
 * Explicit FSM description
 */

fsm_t *fsm_new_cross(int* bp, int* bs, int* e, semaforo_color_t* sp, semaforo_color_t* ss)
{
	/* clang-format off */
	static fsm_trans_t cross_tt[] = {
		{0, state0to1, 1, from0to1},
		{0, state0to0, 0, from0to0},
		{1, state1to1, 1, from1to1},
		{1, state1to2, 2, from1to2},
		{2, state2to2, 2, from2to2},
		{2, state2to3, 3, from2to3},
		{3, state3to4, 4, from3to4},
		{3, state3to3, 3, from3to3},
		{4, state4to4, 4, from4to4},
		{4, state4to5, 5, from4to5},
		{5, state5to5, 5, from5to5},
		{5, state5to0, 0, from5to0},
		{-1, NULL, -1, NULL},
	};
	/* clang-format on */

	fsm_cross_t *this = (fsm_cross_t *)malloc(sizeof(fsm_cross_t));
	fsm_init((fsm_t *)this, cross_tt);
	gettimeofday(&this->next, NULL);
	this->e = e;
	this->bp = bp;
	this->bs = bs;
	this->sp = sp;
	this->ss = ss;
	return (fsm_t *)this;
}