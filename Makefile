NAME = ft_nm
CC = gcc
CFLAGS = -Wall -Werror -Wextra -g

RM = rm -f
LIB = ar rcs

SRC = src/main.c src/aux.c src/ft_struct.c src/format.c src/parsheader.c src/endian_utils.c src/ft_print.c
OBJS = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(OBJS)
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
