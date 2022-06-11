#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "fsm.h"
#include "timer.h"

int button;
int presence_sensor;
int led;
int buzzer;

static int
armed(fsm_t *fsm)
{
	return button;
}

static int
disarmed(fsm_t *fsm)
{
	return !button;
}

static int
presence(fsm_t *fsm)
{
	return presence_sensor;
}

static void
alarm_on(fsm_t *fsm)
{
	led = 1;
	buzzer = 1;
}

static void
turn_on(fsm_t *fsm)
{
}

static void
turn_off(fsm_t *fsm)
{
}

fsm_t *
fsm_new_alarm()
{
	static fsm_trans_t alarm_tt[] = {
		{0, armed, 1, turn_on},
		{1, disarmed, 0, turn_off},
		{1, presence, 1, alarm_on},
		{-1, NULL, -1, NULL},
	};

	return fsm_new(alarm_tt);
}

int main()
{
	struct timeval clk_period = {0, 500 * 1000};
	struct timeval next_activation;
	fsm_t *alarm_fsm = fsm_new_alarm();

	gettimeofday(&next_activation, NULL);
	while (1)
	{
		fsm_fire(alarm_fsm);
		timeval_add(&next_activation, &next_activation, &clk_period);
		delay_until(&next_activation);
	}
}