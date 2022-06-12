/* ISEL 2022 */
/*
* Reto 2 - Paso 1: Alarma basica
*	Cristina Conforto Lopez
*	Marcos Gomez Bracamonte
*	Miguel Medina Anton
*/

/* LTL */

// Si la alarma está activa y se detecta presencia, la alarma suena.
ltl alarmActiveAndPresenceSounds {
    [] (((state == 1) && presence) -> <> [](ledAndBuzzer == 1))
}

// Si la alarma no está activa, la alarma no suena.
ltl alarmNoActiveNoSounds {
     [] ([](state == 0) -> <> [](ledAndBuzzer == 0))
}

// Si la alarma no está activa y se presiona el pulsador, la alarma se activa.
ltl alarmNoActiveAndButtonActive {
    [] (((state == 1) && [](!button)) -> <> [](state == 0)) 
}

// Si la alarma está activa y se presiona el pulsador, la alarma se desactiva.
ltl alarmActiveAndButtonNoActive {
    [] (((state == 0) && [](button)) -> <> [](state == 1))
}


/* Inputs */
bit button;				// 0 = OFF, 1 = ON
bit presence;			// 0 = no presence, 1 = presence detected

/* Outputs */ 
bit ledAndBuzzer;		

byte state;

/* States */
/*
    0 -> Alarm OFF
    1 -> Alarm ON
*/

/* Alarm Finite State Machine */


active proctype alarm_fsm () {
    state = 0;
    ledAndBuzzer = 0;
    do
    :: (state == 0) -> atomic {
        if
        :: button -> state = 1; ledAndBuzzer = presence
        fi
    }
    :: (state == 1)  -> atomic {
        if
        :: !button -> state = 0; ledAndBuzzer = 0
        :: (button && presence) -> ledAndBuzzer = 1
        fi
    }
    od
}

proctype entorno () {
    do
    :: if
       :: button = 0
       :: button = 1
       :: presence = 1
       :: (!button) -> skip
       fi;
       printf ("State = %d, Presence = %d, State = %d, Led and buzzer = %d\n",
               state, presence, button, ledAndBuzzer)
    od
}