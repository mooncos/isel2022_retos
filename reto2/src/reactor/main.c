#include <string.h>
#include <stdio.h>
#include "fsm.h"
#include "reactor.h"
#include "timer.h"
#include "alarm.h"
#include "code.h"
#include "light.h"
#include "interp.h"

static fsm_t *code_fsm;
static fsm_t *alarm_fsm;
static fsm_t *light_fsm;

static void
code_func(struct event_handler_t *this)
{
    fsm_fire(code_fsm);
    struct timeval period = {0, 100 * 1000};
    struct timeval now;
    gettimeofday(&now, NULL);
    timeval_add(&this->next_activation, &period, &now);
}

static void
alarm_func(struct event_handler_t *this)
{
    fsm_fire(alarm_fsm);
    struct timeval period = {0, 250 * 1000};
    struct timeval now;
    gettimeofday(&now, NULL);
    timeval_add(&this->next_activation, &period, &now);
}

static void
light_func(struct event_handler_t *this)
{
    fsm_fire(light_fsm);
    struct timeval period = {0, 500 * 1000};
    struct timeval now;
    gettimeofday(&now, NULL);
    timeval_add(&this->next_activation, &period, &now);
}

int main()
{

    int valid_code = 0;
    int code_button = 0;
    int pir = 0;
    int led = 0;
    int buzzer = 0;
    code_fsm = fsm_new_code (&valid_code, &code_button);
    alarm_fsm = fsm_new_alarm (&valid_code, &pir, &led, &buzzer);
    light_fsm = fsm_new_light (&pir, &led);

    EventHandler eh1, eh2, eh3;
    reactor_init();

    event_handler_init(&eh1, 3, code_func);
    reactor_add_handler(&eh1);

    event_handler_init(&eh2, 2, alarm_func);
    reactor_add_handler(&eh2);

    event_handler_init(&eh3, 1, light_func);
    reactor_add_handler(&eh1);

    while (1)
    {
        reactor_handle_events();
    }
}
