#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"
#include "model.h"

int main()
{
  struct timeval clk_period = {0, 10000 * 1000};
  struct timeval next_activation;
  fsm_t *crossfsm_fsm = fsm_new(crossfsm);

  gettimeofday(&next_activation, NULL);
  while (1)
  {
    fsm_fire(crossfsm_fsm);
    timeval_add(&next_activation, &next_activation, &clk_period);
    delay_until(&next_activation);
  }
}