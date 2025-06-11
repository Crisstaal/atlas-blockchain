CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -I.
LDFLAGS = -lllist -lssl -lcrypto -pthread

SRC = \
	blockchain_create.c \
	block_create.c \
	block_destroy.c \
	blockchain_destroy.c \
	block_hash.c \
	blockchain_serialize.c \
	blockchain_deserialize.c \
	block_is_valid.c

OBJS = $(SRC:.c=.o)
TARGET = libblockchain.a

.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $@ $^

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all