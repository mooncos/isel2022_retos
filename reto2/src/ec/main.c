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
#include "alarm.h"
#include "code.h"
#include "light.h"

int main()
{
    struct timeval next;
    gettimeofday(&next, NULL);
    struct timeval period = {0, 50 * 1000};

    int valid_code = 0;
    int code_button = 0;
    int pir = 0;
    int led = 0;
    int buzzer = 0;
    fsm_t *code_fsm = fsm_new_code(&valid_code, &code_button);
    fsm_t *alarm_fsm = fsm_new_alarm(&valid_code, &pir, &led, &buzzer);
    fsm_t *light_fsm = fsm_new_light(&pir, &led);

    int frame = 0;

    while (1)
    {
        switch (frame)
        {
        case 0:
            fsm_fire(code_fsm);
            fsm_fire(alarm_fsm);
            fsm_fire(light_fsm);
            break;
        case 1:
            break;
        case 2:
            fsm_fire(code_fsm);
            break;
        case 3:
            break;
        case 4:
            fsm_fire(code_fsm);
            break;
        case 5:
            fsm_fire(alarm_fsm);
            break;
        case 6:
            fsm_fire(code_fsm);
            break;
        case 7:
            break;
        case 8:
            fsm_fire(code_fsm);
            break;
        case 9:
            break;
        }
        frame = (frame + 1) % 10;
        timeval_add(&next, &next, &period);
        delay_until(&next);
    }
}