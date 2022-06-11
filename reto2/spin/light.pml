
 
ltl siPresenciaLuz {
	[](( presencia) -> <> (luz))
}

ltl sifinPresenciaLuz {
	[](( (finPresencia) && (!presencia) )-> <> (!luz))
}

/* Inputs */
bit presencia;
bit finPresencia; // 30secs

/* Outputs */
bit luz;

int state=0;


/* Estados*/
/*
    0 -> luz apagada
    1 -> luz encendida y hay presencia
    2 -> luz encendida, no hay presencia y no ha terminado el tiempo de mantener encendido
*/

active proctype fsm(){

    state=0;

    printf("\n----------------------------------\n");
    printf("Estado inicial: 0\n");
    printf("finPresencia: %d, luz: %d, presencia: %d \n", finPresencia, luz, presencia);
    printf("\n----------------------------------\n");

    do
    :: if
        :: (state == 0) -> atomic {
            if
            :: (presencia) -> state = 1; luz = 1; presencia = 0; finPresencia=0; // la parte de la presencia not sure
            :: (!presencia) -> state = 0; luz=0; presencia = 0; finPresencia=0;
            fi
        }
        :: (state == 1) -> atomic {
            if
            :: (presencia) -> state = 1; luz = 1; presencia = 0; finPresencia=0;
            :: (!presencia) -> state = 2; luz = 0; finPresencia=0;
            fi
        }
        :: (state == 2) -> atomic {
            if
            //:: (!finPresencia && !presencia) -> state = 2; luz = 1;  
            :: (finPresencia && !presencia) -> state = 0; luz = 0; finPresencia=0;
            :: (presencia) -> state = 1; luz = 1; presencia=0; 
            fi
        }
        
    fi

    // printf(">> Estado: %d, finPresencia: %d, presencia: %d", state, finPresencia, presencia);
    // printf("   luz: %d \n", luz);

    od

} 

active proctype entorno() {

    printf("\n--------------------------------\n");
    printf("Estado: %e\n", state);
    printf("finPresencia: %d, luz: %d, presencia: %d \n", finPresencia, luz, presencia);
    printf("\n--------------------------------\n");
    do
    :: skip -> skip
    :: presencia = 1;
    :: finPresencia = 1;
        printf("\n--------------------------------\n");
	    printf("Estado: %e\n", state);
	    printf("finPresencia: %d, luz: %d, presencia: %d \n", finPresencia, luz, presencia);
	    printf("\n--------------------------------\n");
    od
}