#file name
NAME			=	custom-shell

#command
CC				=	cc
AR				=	ar
RM				=	rm -rf

#directory
SRC_DIR			=	srcs/
INC_DIR			=	./includes/
LIB_DIR			=	./libcmd
LIB_NAME		=	libcmd.a
LIBCMD			=	$(addprefix $(LIB_DIR)/, $(LIB_NAME))

#srcs
SRCS			=	srcs/main.c srcs/customshell.c srcs/util.c srcs/commandmanager.c srcs/client.c srcs/server.c
OBJS			=	$(SRCS:.c=.o)

all:	$(NAME)

.c.o: 
	$(CC) -c $< -o $@ -I $(INC_DIR)

$(LIBCMD):
	make -C $(LIB_DIR) all

$(NAME): $(OBJS) $(LIBCMD)
	$(CC) -o $(NAME) $(OBJS) -L$(LIB_DIR) -lcmd

clean:
	rm -f $(OBJS) ./srcs/$(NAME)
	make -C $(LIB_DIR) clean

fclean: clean
	make -C $(LIB_DIR) fclean
	$(RM) $(NAME)

re:	fclean all
	make -C $(LIB_DIR) re

.PHONY: all clean fclean re libcmd