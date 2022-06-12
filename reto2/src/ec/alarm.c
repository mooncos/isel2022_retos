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

int *isr_button;
int *isr_presence_sensor;
int *isr_led;
int *isr_buzzer;

static int
armed(fsm_t *fsm)
{
	return *isr_button;
}

static int
disarmed(fsm_t *fsm)
{
	return !*isr_button;
}

static int
presence(fsm_t *fsm)
{
	return *isr_presence_sensor;
}

static void
alarm_on(fsm_t *fsm)
{
	*isr_led = 1;
	*isr_buzzer = 1;
}

static void
turn_on(fsm_t *fsm)
{
	*isr_led = 0;
	*isr_buzzer = 0;
}

static void
turn_off(fsm_t *fsm)
{
	*isr_led = 0;
	*isr_buzzer = 0;
}

fsm_t *
fsm_new_alarm(int *button, int *presence_sensor, int *led, int *buzzer)
{
	static fsm_trans_t alarm_tt[] = {
		{0, armed, 1, turn_on},
		{1, disarmed, 0, turn_off},
		{1, presence, 1, alarm_on},
		{-1, NULL, -1, NULL},
	};
	isr_button = button;
	isr_presence_sensor = presence_sensor;
	isr_led = led;
	isr_buzzer = buzzer;
	return fsm_new(alarm_tt);
}