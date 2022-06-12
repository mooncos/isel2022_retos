#include "stdio.h"

#include "termios.h"
#include "sys/select.h"
#include "sys/time.h"
#include "task.h"

#include "timer.h"

#include "light.h"
#include "alarm.h"
#include "code.h"

static fsm_t *alarm_fsm;
static fsm_t *code_fsm;
static fsm_t *light_fsm;

int valid_code = 0;
int code_button = 0;
int pir = 0;
int led = 0;
int buzzer = 0;

// Alarma
static void alarm_task(void *arg)
{
    alarm_fsm = fsm_new_alarm(&valid_code, &pir, &led, &buzzer);

    struct timeval *period = task_get_period(pthread_self());
    struct timeval next;

    gettimeofday(&next, NULL);
    while (1)
    {
        timeval_add(&next, &next, period);
        delay_until(&next);

        fsm_fire(alarm_fsm);
    }
}

// Codigo
static void code_task(void *arg)
{
    code_fsm = fsm_new_code(&valid_code, &code_button);

    struct timeval *period = task_get_period(pthread_self());
    struct timeval next;

    gettimeofday(&next, NULL);
    while (1)
    {
        timeval_add(&next, &next, period);
        delay_until(&next);

        fsm_fire(code_fsm);
    }
}

// Luz
static void light_task(void *arg)
{
    light_fsm = fsm_new_light(&pir, &led);

    struct timeval *period = task_get_period(pthread_self());
    struct timeval next;

    gettimeofday(&next, NULL);
    while (1)
    {
        timeval_add(&next, &next, period);
        delay_until(&next);

        fsm_fire(light_fsm);
    }
}

int main()
{

    pthread_t alarm_tid, code_tid, light_tid;

    code_tid = task_new("codigo", (void*) code_task, 100, 100, 3, 2048);
    alarm_tid = task_new("alarma", (void*) alarm_task, 250, 250, 2, 2048);
    light_tid = task_new("luz", (void*) light_task, 500, 500, 1, 2048);

    pthread_join(code_tid, NULL);
    pthread_join(alarm_tid, NULL);
    pthread_join(light_tid, NULL);

    return 0;
}