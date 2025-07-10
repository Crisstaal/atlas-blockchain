CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -I. -I../../crypto -Itransaction/
LDFLAGS = -L../../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread

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
	blockchain_difficulty.c \
	hash_matches_difficulty.c \
	transaction/tx_out_create.c \
	transaction/unspent_tx_out_create.c

OBJS = $(SRC:.c=.o)
TARGET = libhblk_blockchain_v0.3.a

.PHONY: all clean fclean re

all: $(TARGET)

# Compile object files into archive
$(TARGET): $(OBJS)
	ar rcs $@ $^

# Build the crypto library before our target
libhblk_crypto:
	$(MAKE) -C ../../crypto libhblk_crypto.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all
