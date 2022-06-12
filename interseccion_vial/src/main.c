#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "fsm.h"
#include "model.h"


fsm_t* fsm_new_cross(int* bp, int* bs, int* e, semaforo_color_t* sp, semaforo_color_t* ss);
fsm_t* fsm_new_interp(int* boton1, int* boton2);

int main()
{
  int botonp = 0;
  int botons = 0;
  int espira = 0;
  semaforo_color_t semaforoPrincipal = VERDE;
  semaforo_color_t semaforoSecundario = ROJO;
  struct timeval clk_period = {0, 500 * 1000};
  struct timeval next_activation;
  fsm_t *crossfsm_fsm = fsm_new_cross(&botonp, &botons, &espira, &semaforoPrincipal, &semaforoSecundario);
  fsm_t *interp_fsm = fsm_new_interp(&botonp, &botons);

  gettimeofday(&next_activation, NULL);
  while (1)
  {
    fsm_fire(crossfsm_fsm);
    fsm_fire(interp_fsm);
    timeval_add(&next_activation, &next_activation, &clk_period);
    delay_until(&next_activation);
  }
}
