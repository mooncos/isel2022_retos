#include "model.h"
#include <stdlib.h>
#include <string.h>

static char cmd[40];

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

static int cmd_BotPri(fsm_t *fsm)
{
    return (strcmp(cmd, "bp") == 0);
}

static int cmd_botSec(fsm_t *fsm)
{
    return (strcmp(cmd, "bs") == 0);
}

static int cmd_espiraAct(fsm_t *fsm)
{
    return (strcmp(cmd, "e") == 0);
}

static int cmd_quit(fsm_t *fsm)
{
    return (strcmp(cmd, "q") == 0);
}

static void do_quit(fsm_t *this)
{
    exit(0);
}

static void do_botSec(fsm_t *this)
{
    botonCruceSecundario = 1;
}

static void do_botPri(fsm_t *this)
{
    botonCrucePrincipal = 1;
}

static void do_espiraAct(fsm_t *this)
{
    espira = 1;
}

fsm_t *fsm_new_interp(void)
{
    static fsm_trans_t interp_tt[] = {
        {0, input_ready, 0, do_read},
        {0, cmd_BotPri, 0, do_botPri},
        {0, cmd_botSec, 0, do_botSec},
        {0, cmd_espiraAct, 0, do_espiraAct},
        {0, cmd_quit, 0, do_quit},
        {-1, NULL, -1, NULL},
    };

    return fsm_new(interp_tt);
}