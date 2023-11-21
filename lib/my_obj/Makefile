NAME 	=	libmy_obj.a

SRCS  = ./src/my_obj.c \

OBJS	=	$(SRCS:.c=.o)

AR = ar rc

RM	=	rm -f

CFLAGS = -I ./include/
CFLAGS += -W -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS)
		@ $(AR) $(NAME) $(OBJS)

clean:
		@ $(RM) $(OBJS)

fclean:	clean
		@ $(RM) $(NAME)

re: fclean all

%.o:	%.c
	@ $(CC) -o $@ -c $< $(CFLAGS)

.PHONY: all clean fclean re