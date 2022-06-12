#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <assert.h>
#include "fsm.h"
#include "light.h"
#include "timer.h"

#define FIN_PRESENCIA_SEC 30

<<<<<<< Updated upstream
static struct timespec now;
static struct timespec end;
static struct timespec T = {FIN_PRESENCIA_SEC, 0};
=======
static struct timeval now;
static struct timeval end;
static struct timeval T = {TIMER_PERIOD_SEG, 0};
>>>>>>> Stashed changes

int *luz;
volatile int *presencia;

static int is_presencia(fsm_t *fsm)
{
	return *presencia;
}

static void light_on(fsm_t *fsm)
{
	*luz = 1;
	struct timeval now;
	gettimeofday(&now, NULL);
	timeval_add(&end, &now, &T);
}

static void light_off(fsm_t *fsm)
{
	*luz = 0;
}

static int finPresencia(fsm_t *this)
{
	gettimeofday(&now, NULL);
	return timeval_less(&end, &now);
}

fsm_t *fsm_new_light(volatile int *button, int *light)
{

	static fsm_trans_t light_tt[] = {
		{0, is_presencia, 1, light_on},
		{1, is_presencia, 1, light_on},
		{1, finPresencia, 0, light_off},
		{-1, NULL, -1, NULL},
	};

	presencia = button;
	luz = light;

	return fsm_new(light_tt);
}