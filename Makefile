NAME	:= ircserv
CC		:= clang++

BUILD_COMMIT_SHA 	:= $(shell git rev-parse --short HEAD)

OBJ_DIR	:= ./obj/
SRC_DIR	:= ./src/

DBFLAGS = -pedantic -Wunreachable-code -Wunused
BFLAGS = -D BUILD_COMMIT_SHA=\"@$(BUILD_COMMIT_SHA)\"
CFLAGS	:= -Wall -Wextra -Werror -I $(SRC_DIR) -std=c++98

DFLAGS	= -MMD -MF $(@:.o=.d)
SHELL	:= /bin/bash

FILES	= main.cpp \
	command/cap.cpp command/command.cpp command/hello.cpp command/join.cpp command/motd.cpp \
	command/lusers.cpp command/mode.cpp command/whois.cpp \
	command/nick.cpp command/user.cpp command/pass.cpp command/quit.cpp \
	command/ping.cpp command/part.cpp command/privmsg.cpp \
	command/notice.cpp command/oper.cpp command/invite.cpp command/kick.cpp \
	command/kill.cpp command/list.cpp command/names.cpp command/time.cpp command/topic.cpp \
	engine/server.cpp engine/parser.cpp \
	client/client.cpp

SRCS	:=	$(addprefix $(SRC_DIR), $(FILES))
OBJS	:=	$(addprefix $(OBJ_DIR), $(FILES:.cpp=.o))
DEPS	:=	$(addprefix $(OBJ_DIR), $(FILES:.cpp=.d))

.PHONY	: all
all		: $(NAME)

-include $(DEPS)
$(NAME)	: $(OBJS)
	@	printf "Compiling $(NAME)\n"
	@	$(CC) $(CFLAGS) $(CXXFLAGS) $(BFLAGS) $^ -o $@ -g3

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@	mkdir -p $(dir $@)
	@	printf "Compiling: $<"
	@	$(CC) $(CFLAGS) $(CXXFLAGS) $(BFLAGS) -c $< -o $@ $(DFLAGS) -g3
	@	printf " -> OK\n"

.PHONY	: clean
clean	:
	@	printf "Deleting $(OBJ_DIR)\n"
	@	rm -rf $(OBJ_DIR)

.PHONY	: fclean
fclean	: clean
	@	printf "Deleting $(NAME) binary\n"
	@	rm -f $(NAME)

.PHONY	: test
test	: all

.PHONY	: client1
client1	:
	docker build -t irssi1 -f irssi/Dockerfile1 irssi
	docker run --net=host -it irssi1

.PHONY	: client2
client2	:
	docker build -t irssi2 -f irssi/Dockerfile2 irssi
	docker run --net=host -it irssi2

.PHONY	: re
re		: fclean all

.PHONY	: lint
lint	:
	@	cpplint --recursive --root=srcs \
		--filter=-whitespace/tab,-legal/copyright srcs/