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
	block_is_valid.c \
	block_mine.c \
	hash_matches_difficulty.c

OBJS = $(SRC:.c=.o)
TARGET = libhblk_blockchain_v0.2.a

.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all
