#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "fsm.h"

static char cmd[40];

struct fsm_interp_t
{
    fsm_t fsm;
    int *boton[3];
};
typedef struct fsm_interp_t fsm_interp_t;

static int input_ready(fsm_t *fsm)
{
    int ret;
    fd_set rds;
    struct timeval timeout = {0, 0};
    FD_ZERO(&rds);
    FD_SET(0, &rds);
    ret = select(1, &rds, NULL, NULL, &timeout);
    return ret;
}
static void do_read(fsm_t *fsm)
{
    int cnt = read(0, cmd, 40);
    cmd[cnt - 1] = '\0';
}

static int cmd_b(fsm_t *fsm) { return strncmp(cmd, "b", 1) == 0; }
static void do_b(fsm_t *fsm)
{
    fsm_interp_t *this = (fsm_interp_t *)fsm;
    char b = *(cmd + 1);
    if (b == 'p')
    {
        *(this->boton[0]) = 1;
    }
    else if (b == 's')
    {
        *(this->boton[1]) = 1;
    }
    cmd[0] = '\0';
}

static int cmd_e(fsm_t *fsm) { return strncmp(cmd, "e", 1) == 0; }
static void do_e(fsm_t *fsm)
{
    fsm_interp_t *this = (fsm_interp_t *)fsm;
    *(this->boton[2]) = 1;

    cmd[0] = '\0';
}

static int cmd_help(fsm_t *fsm) { return strcmp(cmd, "h") == 0; }
static void do_help(fsm_t *this)
{
    printf(
        "b[p,s] \tSimula pulsación del botón para cruzar\n"
        "q \tSalir\n"
        "h \tMuestra esta ayuda\n");
    cmd[0] = '\0';
}

static int cmd_quit(fsm_t *fsm) { return strcmp(cmd, "q") == 0; }
static void do_quit(fsm_t *this) { exit(0); }

fsm_t *
fsm_new_interp(int *boton1, int *boton2, int *espira)
{
    static fsm_trans_t tt[] = {
        {0, input_ready, 0, do_read},
        {0, cmd_b, 0, do_b},
        {0, cmd_e, 0, do_e},
        {0, cmd_help, 0, do_help},
        {0, cmd_quit, 0, do_quit},
        {-1, NULL, -1, NULL},
    };
    fsm_interp_t *this = (fsm_interp_t *)malloc(sizeof(fsm_interp_t));
    fsm_init((fsm_t *)this, tt);
    this->boton[0] = boton1;
    this->boton[1] = boton2;
    this->boton[2] = espira;
    return (fsm_t *)this;
}
