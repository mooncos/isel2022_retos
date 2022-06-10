#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"
#include "model.h"

/* Actuators */
semaforo_color_t semaforoPrincipal = VERDE;
semaforo_color_t semaforoSecundario = ROJO;

/* Sensors */
botonCrucePrincipal = 0;
botonCruceSecundario = 0;
espira = 0;

/* Timer flags */
struct timeval esperaLarga = {30, 0}; // 30 segundos
struct timeval esperaAmbar = {5, 0};  // 5 segundos
struct timeval esperaCambio = {1, 0}; // 1 segundos
struct timeval next = {0, 0};

/*
 * Utility functions

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
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&next, &now);
	return (finEsperaLarga && (espira || botonCrucePrincipal));
}
static int state0to0(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&next, &now);
	return (!finEsperaLarga || (!espira && !botonCrucePrincipal));
}

static int state1to1(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&next, &now);
	return (!finEsperaAmbar);
}
static int state1to2(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&next, &now);
	return (finEsperaAmbar);
}

static int state2to2(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&next, &now);
	return (!finEsperaCambio);
}
static int state2to3(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&next, &now);
	return (finEsperaCambio);
}

static int state3to4(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&next, &now);
	return (finEsperaLarga || (!espira && !botonCruceSecundario));
}
static int state3to3(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaLarga = timeval_less(&next, &now);
	return (!finEsperaLarga && (espira || botonCruceSecundario));
}

static int state4to4(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&next, &now);
	return (!finEsperaAmbar);
}
static int state4to5(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaAmbar = timeval_less(&next, &now);
	return (finEsperaAmbar);
}

static int state5to5(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&next, &now);
	return (!finEsperaCambio);
}
static int state5to0(fsm_t *this)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	int finEsperaCambio = timeval_less(&next, &now);
	return (finEsperaCambio);
}

/* Action functions */
static void from0to1(fsm_t *this)
{
	espira = 0;
	botonCrucePrincipal = 0;
	semaforoPrincipal = AMBAR;
	semaforoSecundario = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&next, &now, &esperaAmbar);
	printf(">> From 0 to 1:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}
static void from0to0(fsm_t *this)
{
	semaforoPrincipal = VERDE;
	semaforoSecundario = ROJO;
	printf(">> From 0 to 0:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}

static void from1to1(fsm_t *this)
{
	semaforoPrincipal = AMBAR;
	semaforoSecundario = ROJO;
	printf(">> From 1 to 1:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}
static void from1to2(fsm_t *this)
{
	semaforoPrincipal = ROJO;
	semaforoSecundario = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&next, &now, &esperaCambio);
	printf(">> From 1 to 2:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}

static void from2to2(fsm_t *this)
{

	semaforoPrincipal = ROJO;
	semaforoSecundario = ROJO;
	printf(">> From 2 to 2:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}
static void from2to3(fsm_t *this)
{
	semaforoPrincipal = ROJO;
	semaforoSecundario = VERDE;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&next, &now, &esperaLarga);
	printf(">> From 2 to 3:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}

static void from3to4(fsm_t *this)
{
	semaforoPrincipal = ROJO;
	semaforoSecundario = AMBAR;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&next, &now, &esperaAmbar);
	printf(">> From 3 to 4:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}
static void from3to3(fsm_t *this)
{
	espira = 0;
	botonCruceSecundario = 0;
	semaforoPrincipal = ROJO;
	semaforoSecundario = VERDE;
	printf(">> From 3 to 3:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}

static void from4to4(fsm_t *this)
{

	semaforoPrincipal = ROJO;
	semaforoSecundario = AMBAR;
	printf(">> From 4 to 4:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}
static void from4to5(fsm_t *this)
{
	semaforoPrincipal = ROJO;
	semaforoSecundario = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&next, &now, &esperaCambio);
	printf(">> From 4 to 5:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}

static void from5to5(fsm_t *this)
{

	semaforoPrincipal = ROJO;
	semaforoSecundario = ROJO;
	printf(">> From 5 to 5:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}
static void from5to0(fsm_t *this)
{
	semaforoPrincipal = VERDE;
	semaforoSecundario = ROJO;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&next, &now, &esperaLarga);
	printf(">> From 5 to 0:\n  sP: %d\n  sS: %d\n  next: %ld.%06ld\n", semaforoPrincipal, semaforoSecundario, next.tv_sec, next.tv_usec);
}

/*
 * Explicit FSM description
 */

fsm_t *fsm_new_cross(void)
{

	// clang-format off
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
	// clang-format on

	return fsm_new(cross_tt);
}