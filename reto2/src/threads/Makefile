CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG -pthread

all: main

alarm: alarm.o fsm.o timer.o

light: light.o fsm.o timer.o

code: code.o fsm.o timer.o

main: main.o task.o timer.o alarm.o light.o code.o fsm.o

clean:
	$(RM) *.o *~ main
