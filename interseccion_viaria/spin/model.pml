/* LTL specs */

/* 
    Semaforo para coches estados:
    2 -> verde
    1 -> ambar
    0 -> rojo
*/
ltl peatonEvtCrucePrincipal {
	[]((botonCrucePrincipal && ([]<> finEsperaLarga) && ([]<> finEsperaAmbar) && ([]<>finEsperaCambio)) -> (<> (semaforoPrincipal == 0)))
}

ltl peatonEvtCruceSecundario {
	[]((botonCruceSecundario && ([]<> finEsperaLarga) && ([]<> finEsperaAmbar) && ([]<>finEsperaCambio)) -> (<> (semaforoSecundario == 0)))
}

ltl cochesEspiraPasarSecundario {
	[]((espira && ([]<> finEsperaLarga) && ([]<> finEsperaAmbar) && ([]<>finEsperaCambio)) -> (<> (semaforoSecundario == 2)))
}

ltl nuncaDosSemaforosVerde {
    []!((semaforoPrincipal == 2) && (semaforoSecundario == 2))
}


/* Inputs */
bit espira;
bit botonCrucePrincipal;
bit botonCruceSecundario;
bit finEsperaLarga; // 30 segundos
bit finEsperaAmbar; // 5 segundos
bit finEsperaCambio; // 1 segundos

/* Outputs */
int semaforoPrincipal;
int semaforoSecundario; 

/* Estados */
/*
    0 -> Verde principal, rojo secundario
    1 -> Ambar principal, rojo secundario
    2 -> Rojo principal, rojo secundario (estado de cambio de tráfico a secundaria)
    3 -> Rojo principal, verde secundario
    4 -> Rojo principal, ambar secundario
    5 -> Rojo principal, rojo secundario (estado de cambio de tráfico a principal)
*/

active proctype fsm() {
    int state = 0;
    
    do
    :: if
        :: (state == 0) -> atomic {
            if
            :: (finEsperaLarga && (espira || botonCrucePrincipal)) -> state = 1; finEsperaLarga = 0; espira = 0; botonCrucePrincipal = 0; semaforoPrincipal = 1; semaforoSecundario = 0;
            :: (!finEsperaLarga || (!espira && !botonCrucePrincipal)) -> state = 0; semaforoPrincipal = 2; semaforoSecundario = 0;
            fi
        }
        :: (state == 1) -> atomic {
            if
            :: (!finEsperaAmbar) -> state = 1; semaforoPrincipal = 1; semaforoSecundario = 0;
            :: (finEsperaAmbar) -> state = 2; finEsperaAmbar = 0; semaforoPrincipal = 0; semaforoSecundario = 0;
            fi
        }
        :: (state == 2) -> atomic {
            if
            :: (!finEsperaCambio) -> state = 2; semaforoPrincipal = 0; semaforoSecundario = 0;
            :: (finEsperaCambio) -> state = 3; finEsperaCambio = 0; semaforoPrincipal = 0; semaforoSecundario = 2;
            fi
        }
        :: (state == 3) -> atomic {
            if
            :: (finEsperaLarga || (!espira && !botonCruceSecundario)) -> state = 4; finEsperaLarga = 0; semaforoPrincipal = 0; semaforoSecundario = 1;
            :: (!finEsperaLarga && (espira || botonCruceSecundario)) -> state = 3; espira = 0; botonCruceSecundario = 0; semaforoPrincipal = 0; semaforoSecundario = 2;
            fi
        }
        :: (state == 4) -> atomic {
            if
            :: (!finEsperaAmbar) -> state = 4; semaforoPrincipal = 0; semaforoSecundario = 1;
            :: (finEsperaAmbar) -> state = 5; finEsperaAmbar = 0; semaforoPrincipal = 0; semaforoSecundario = 0;
            fi
        }
        :: (state == 5) -> atomic {
            if
            :: (!finEsperaCambio) -> state = 5; semaforoPrincipal = 0; semaforoSecundario = 0;
            :: (finEsperaCambio) -> state = 0; finEsperaCambio = 0; semaforoPrincipal = 2; semaforoSecundario = 0;
            fi
        }
    fi
    
    od

    }

active proctype entorno() {
    do
    :: skip -> skip
    :: espira = 1;
    :: finEsperaCambio = 1;
    :: finEsperaLarga = 1;
    :: finEsperaAmbar = 1;
    :: botonCrucePrincipal = 1;
    :: botonCruceSecundario = 1;
    od
}