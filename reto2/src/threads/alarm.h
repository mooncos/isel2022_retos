#ifndef _ALARM_H_
#define _ALARM_H_

#include "fsm.h"

//FSM DEFINITION
fsm_t* fsm_new_alarm (int *button, int *presence_sensor, int *led, int *buzzer);

#endif