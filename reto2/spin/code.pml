/* ISEL 2022 */
/*
* Reto 2 - Paso 2: Alarma con código
*	Cristina Conforto Lopez
*	Marcos Gomez Bracamonte
*	Miguel Medina Anton
*/

ltl codeKOWhileInputNotCode {
    [] (((code_in[0] != code_word[0]) ||
         (code_in[1] != code_word[1]) ||
         (code_in[2] != code_word[2])) -> (step != 3))
}

#define timeout true

bit button;
bit code_ok;
bit begin;
bit timeout_reset;

byte code_word[3] = {1, 2, 3};
byte code_in[4];
int step;

active proctype alarm_fsm () {
    do
    :: code_ok -> code_ok = 0
    od
}

active proctype code_fsm () {
    code_ok = 0;
    step = 0;
    timeout_reset = 0;
    do
    :: true -> atomic {
        if
        :: (step >= 3) ->
            code_ok = 1; step = 0

        :: (!timeout_reset && timeout && begin && (step < 3)
            && (code_in[step] == code_word[step])) ->
            step++; code_in[step] = 0

        :: (!timeout_reset && timeout && begin && (step < 3)
            && (code_in[step] != code_word[step])) ->
            step = 0; code_in[step] = 0

        :: (!timeout_reset && button && (step < 3)) ->
            code_in[step] = (code_in[step] + 1) % 10;
            button = 0; begin = 1

        :: (timeout_reset) ->
            step = 0; code_in[step] = 0; timeout_reset = 0
        fi
    }
    od
}

active proctype entorno () {
    do
    :: if
       :: button = 1
       :: (!button) -> skip
       fi ;
       printf ("button = %d, code_in=%d%d%d [%d], code_ok = %d\n",
               button, code_in[0], code_in[1], code_in[2], step, code_ok)
    od
}