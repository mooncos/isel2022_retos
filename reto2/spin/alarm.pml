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
    [](((alarm == 1) && presence) -> <>((alarm == 2) && buzzer && led))
}

// Si la alarma no está activa, la alarma no suena.
ltl alarmNoActiveNoSounds {
    []((alarm == 0 && presence) -> (!buzzer && !led))
}

// Si la alarma no está activa y se presiona el pulsador, la alarma se activa.
ltl alarmNoActiveAndButtonActive {
    [](((alarm == 0) && button) -> <>(alarm == 1))
}

// Si la alarma está activa y se presiona el pulsador, la alarma se desactiva.
ltl alarmActiveAndButtonNoActive {
    []((((alarm == 1) || (alarm == 2)) && button) -> <>((alarm == 0) && !buzzer && !led))
}

/* Alarm Finite State Machine */


// Inputs
bit presence = 0;			// 0 = no presence, 1 = presence
bit button = 0; 			// 0 = not pressed, 1 = pressed

// Outputs
bit led = 0;				// 0 = OFF, 1 = ON
bit buzzer = 0; 			// 0 = OFF, 1 = ON

/* States */
/*
    0 -> Alarm OFF
    1 -> Alarm ON
    2 -> Presence detected
*/

int alarm;

active proctype alarm_fsm() {
	
	alarm = 0;
	led = 0;
	buzzer = 0;

	printf("++ Initial State 0\n");
    
    do
    :: if
        :: (alarm == 0) -> atomic {
            if
            :: (button == 1) -> alarm = 1; led = 0; buzzer = 0; button = 0; presence = 0;
            fi
        }
        :: (alarm == 1) -> atomic {
            if
            :: (button == 1) -> alarm = 0; led = 0; buzzer = 0; button = 0; presence = 0;
            :: (presence == 1) -> alarm = 2; led = 1; buzzer = 1; button = 0; presence = 0;
            fi
        }
        :: (alarm == 2) -> atomic {
            if
            :: (button == 1) -> alarm = 0; led = 0; buzzer = 0; button = 0; presence = 0;
            fi
        }
    fi

    printf(">> State: %d, button: %d, presence: %d\n", alarm, button, presence);
    printf("   led: %d, buzzer: %d\n\n",  led, buzzer);

    od
}

active proctype entorno() {
    do
    :: skip -> skip
    :: button = 1
    :: presence = 1
    od
}
