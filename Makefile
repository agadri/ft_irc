SRCS1         =    server.cpp
SRCS2         = client.cpp

OBJS1         =     ${SRCS1:.cpp=.o}
OBJS2        =     ${SRCS2:.cpp=.o}

CC             =     c++
CFLAGS        =     -Wall -Wextra -Werror -std=c++98 -g
RM            =     rm -f

all:             ${NAME}

.cpp.o:
				${CC} ${CFLAGS} -c $< -o ${<:.cpp=.o}


NAME1        =     server
NAME2        =     client

all:             ${NAME1} ${NAME2}
				
$(NAME1):         $(OBJS1)
				${CC} $(CFLAGS) $(OBJS1) -o $(NAME1)
$(NAME2):         $(OBJS2)
				${CC} $(CFLAGS) $(OBJS2) -o $(NAME2)

clean:
				${RM} ${OBJS1} ${OBJS2}

fclean:         clean
				${RM} ${NAME1} ${NAME2}

re:             fclean all

.PHONY:            all clean fclean re% 