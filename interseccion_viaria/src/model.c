#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"

/* Actuators */
static int semaforoPrincipal = 1;
static int semaforoSecundario = 0;

/* Sensors */
static int botonCrucePrincipal = 0;
static int botonCruceSecundario = 0;
static int espira = 0;

/* Timer flags */
int finEsperaLarga;	 // 30 segundos
int finEsperaAmbar;	 // 5 segundos
int finEsperaCambio; // 1 segundos

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
	return (finEsperaLarga && (espira || botonCrucePrincipal));
}
static int state0to0(fsm_t *this)
{
	return (!finEsperaLarga || (!espira && !botonCrucePrincipal));
}

static int state1to1(fsm_t *this)
{
	return (!finEsperaAmbar);
}
static int state1to2(fsm_t *this)
{
	return (finEsperaAmbar);
}

static int state2to2(fsm_t *this)
{
	return (!finEsperaCambio);
}
static int state2to3(fsm_t *this)
{
	return (finEsperaCambio);
}

static int state3to4(fsm_t *this)
{
	return (finEsperaLarga || (!espira && !botonCruceSecundario));
}
static int state3to3(fsm_t *this)
{
	return (!finEsperaLarga && (espira || botonCruceSecundario));
}

static int state4to4(fsm_t *this)
{
	return (!finEsperaAmbar);
}
static int state4to5(fsm_t *this)
{
	return (finEsperaAmbar);
}

static int state5to5(fsm_t *this)
{
	return (!finEsperaCambio);
}
static int state5to0(fsm_t *this)
{
	return (finEsperaCambio);
}

/* Action functions */
static void from0to1(fsm_t *this)
{

	finEsperaLarga = 0;
	espira = 0;
	botonCrucePrincipal = 0;
	semaforoPrincipal = 1;
	semaforoSecundario = 0;
}
static void from0to0(fsm_t *this)
{
	semaforoPrincipal = 2;
	semaforoSecundario = 0;
}

static void from1to1(fsm_t *this)
{

	semaforoPrincipal = 1;
	semaforoSecundario = 0;
}
static void from1to2(fsm_t *this)
{
	finEsperaAmbar = 0;
	semaforoPrincipal = 0;
	semaforoSecundario = 0;
}

static void from2to2(fsm_t *this)
{

	semaforoPrincipal = 0;
	semaforoSecundario = 0;
}
static void from2to3(fsm_t *this)
{
	finEsperaCambio = 0;
	semaforoPrincipal = 0;
	semaforoSecundario = 2;
}

static void from3to4(fsm_t *this)
{

	finEsperaLarga = 0;
	semaforoPrincipal = 0;
	semaforoSecundario = 1;
}
static void from3to3(fsm_t *this)
{
	espira = 0;
	botonCruceSecundario = 0;
	semaforoPrincipal = 0;
	semaforoSecundario = 2;
}

static void from4to4(fsm_t *this)
{

	semaforoPrincipal = 0;
	semaforoSecundario = 1;
}
static void from4to5(fsm_t *this)
{
	finEsperaAmbar = 0;
	semaforoPrincipal = 0;
	semaforoSecundario = 0;
}

static void from5to5(fsm_t *this)
{

	semaforoPrincipal = 0;
	semaforoSecundario = 0;
}
static void from5to0(fsm_t *this)
{
	finEsperaCambio = 0;
	semaforoPrincipal = 2;
	semaforoSecundario = 0;
}

/*
 * Explicit FSM description
 */
static fsm_trans_t crossfsm[] = {
	{0,
	 state0to1,
	 1,
	 from0to1},
	{0,
	 state0to0,
	 0,
	 from0to0},
	{1,
	 state1to1,
	 1,
	 from1to1},
	{1,
	 state1to2,
	 2,
	 from1to2},
	{2,
	 state2to2,
	 2,
	 from2to2},
	{2,
	 state2to3,
	 3,
	 from2to3},
	{3,
	 state3to4,
	 4,
	 from3to4},
	{3,
	 state3to3,
	 3,
	 from3to3},
	{4,
	 state4to4,
	 4,
	 from4to4},
	{4,
	 state4to5,
	 5,
	 from4to5},
	{5,
	 state5to5,
	 5,
	 from5to5},
	{5,
	 state5to0,
	 0,
	 from5to0},
	{-1, NULL, -1, NULL},
};