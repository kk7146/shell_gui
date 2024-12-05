SRCS	= cmd_cat.c cmd_cd.c cmd_chmod.c cmd_cp.c cmd_kill.c cmd_ln.c cmd_ls.c cmd_mkdir.c cmd_ps.c cmd_rename.c cmd_rm.c cmd_rmdir.c cmd_exec.c
OBJS	= $(SRCS:.c=.o)
NAME	= libcmd.a

all: $(NAME)

$(NAME) : $(OBJS)
	ar rc $(NAME) $(OBJS)
	touch $@

.c.o:
	cc -c $< -o $(<:.c=.o) -I ../includes

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
