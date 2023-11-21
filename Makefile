NAME 	=	m_obj

SRCS  =	./src/main.c \

OBJS	=	$(SRCS:.c=.o)

CC	=	gcc

RM	=	rm -f

CFLAGS = -I ./lib/my_obj/include/
CFLAGS += -W -Wall -Wextra

LIB_FOLDER = ./lib/my_obj

MYLIB = -L $(LIB_FOLDER) -lmy_obj \


all: $(NAME)

$(NAME): $(OBJS)
		make -s -C $(LIB_FOLDER)
		$(CC) -o $(NAME) $(OBJS) $(MYLIB)

clean:
		$(RM) $(OBJS)

fclean:	clean
		@ make fclean -s -C $(LIB_FOLDER)

re: fclean all

%.o:	%.c
	@ $(CC) -o $@ -c $< $(CFLAGS)

.PHONY: all clean fclean re