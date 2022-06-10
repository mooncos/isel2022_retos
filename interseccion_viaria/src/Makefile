NAME	= cross
CC		= gcc
CFLAGS	= -Wall -g
SRCS 	= fsm.c interp.c main.c 
OBJS	= ${SRCS:.c=.o}
RM		= rm -f

all: ${NAME}

%o: %c
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

$(NAME) : ${OBJS}
	ar rc ${NAME} ${OBJS}

clean:
		${RM} ${OBJS}

fclean:		clean
		${RM} ${NAME}
re:		fclean all
.PHONY:		clean re fclean all