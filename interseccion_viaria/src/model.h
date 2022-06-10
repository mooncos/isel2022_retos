#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"

int botonCrucePrincipal, botonCruceSecundario, espira;

typedef enum
{
    ROJO,
    AMBAR,
    VERDE
} semaforo_color_t;

void timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b);
void timeval_add(struct timeval *res, struct timeval *a, struct timeval *b);
int timeval_less(struct timeval *a, struct timeval *b);
void delay_until(struct timeval *next_activation);
