#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"

typedef enum
{
    ROJO,
    AMBAR,
    VERDE
} semaforo_color_t;

struct fsm_cross_t {
	fsm_t fsm;
	int* bp;
    int* bs;
    int* e;
    semaforo_color_t* sp;
    semaforo_color_t* ss;
	struct timeval next;
};
typedef struct fsm_cross_t fsm_cross_t;

void timeval_sub(struct timeval *res, struct timeval *a, struct timeval *b);
void timeval_add(struct timeval *res, struct timeval *a, struct timeval *b);
int timeval_less(struct timeval *a, struct timeval *b);
void delay_until(struct timeval *next_activation);