#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "fsm.h"
#include "light.h"

#define TIMER_PERIOD_SEG 30

static struct timespec now;
static struct timespec end;
static struct timespec T = {TIMER_PERIOD_SEG, 0};

int luz;
int presencia;

static int presencia(fsm_t *fsm) {
	return presencia;
}

static void light_on(fsm_t *fsm)
{
	luz = 1;
}

static void light_off(fsm_t *fsm) {
    luz=0;
}

static int finPresencia (fsm_t* this) {
  clock_gettime(CLOCK_REALTIME, &now); 
  return timespec_less (&end, &now); 
}

fsm_t * fsm_light() {
	static fsm_trans_t alarm_tt[] = {
		{0, presencia, 1, light_on},
		{1, presencia, 1, light_on},
		{1, finPresencia, 0, light_off},
		{-1, NULL, -1, NULL},
	};

	return fsm_new(light_tt);
}

int main()
{
	struct timeval clk_period = {0, 500 * 1000};
	struct timeval next_activation;
	fsm_t *light_fsm = fsm_new_light();

	gettimeofday(&next_activation, NULL);
	while (1)
	{
		fsm_fire(light_fsm);
		timeval_add(&next_activation, &next_activation, &clk_period);
		delay_until(&next_activation);
	}
}