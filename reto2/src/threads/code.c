#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "fsm.h"
#include "code.h"
#include "timer.h"


static int secret[] = {1, 2, 3};
static int input[10];
static int step = 0;
static struct timeval button_next;
static struct timeval button_period = {1, 0};

static int *valid_code = NULL;

static volatile int *button = NULL;
static int started = 0;

static int
code_ok(fsm_t *fsm)
{
    return step >= (sizeof(secret) / sizeof(secret[0]));
}

static int
timeout(fsm_t *fsm)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return !code_ok(fsm) && started && (timeval_less(&button_next, &now));
}

static int
timeout_and_digit_ok(fsm_t *fsm)
{
    return timeout(fsm) && !code_ok(fsm) && (input[step] == secret[step]);
}

static int
timeout_and_digit_not_ok(fsm_t *fsm)
{
    return timeout(fsm) && !code_ok(fsm) && (input[step] != secret[step]);
}

static int
button_pressed(fsm_t *fsm)
{
    return !code_ok(fsm) && !timeout(fsm) && button;
}

static void
increment_current_digit(fsm_t *fsm)
{
    input[step] = (input[step] + 1) % 10;
    struct timeval now;
    gettimeofday(&now, NULL);
    timeval_add(&button_next, &now, &button_period);
    started = 1;
    button = 0;
}

static void
next_digit(fsm_t *fsm)
{
    input[++step] = 0;
    started = 0;
}

static void
reset(fsm_t *fsm)
{
    printf("CODE: RESET %d (%d/%d, %d/%d, %d/%d)\n", step,
           input[0], secret[0], input[1], secret[1], input[2], secret[2]);
    step = 0;
    input[step] = 0;
    started = 0;
}

static void
accept_code(fsm_t *fsm)
{
    *valid_code = 1;
    reset(fsm);
    printf("CODE OK\n");
}

fsm_t *
fsm_new_code(int *valid_codep, volatile int *bt)
{
    static fsm_trans_t code_tt[] = {
        {0, code_ok, 0, accept_code},
        {0, timeout_and_digit_ok, 0, next_digit},
        {0, timeout_and_digit_not_ok, 0, reset},
        {0, button_pressed, 0, increment_current_digit},
        {-1, NULL, -1, NULL},
    };
    valid_code = valid_codep;
    button = bt;
    return fsm_new(code_tt);
}