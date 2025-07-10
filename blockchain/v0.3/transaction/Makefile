CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -g3 -I. -Itransaction -Iprovided -I../../crypto
AR = ar
ARFLAGS = rcs

SRC = \
	blockchain_create.c \
	block_create.c \
	block_destroy.c \
	blockchain_destroy.c \
	block_hash.c \
	blockchain_serialize.c \
	blockchain_deserialize.c \
	block_is_valid.c \
	hash_matches_difficulty.c \
	blockchain_difficulty.c \
	block_mine.c \
	transaction/tx_out_create.c \
	transaction/unspent_tx_out_create.c \
	transaction/tx_in_create.c \
	transaction/transaction_hash.c \
	transaction/tx_in_sign.c \
	transaction/transaction_create.c \
	transaction/transaction_is_valid.c \
	transaction/coinbase_create.c \
	transaction/coinbase_is_valid.c \
	transaction/transaction_destroy.c \
	transaction/update_unspent.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean re

all: libhblk_blockchain.a

libhblk_blockchain.a: $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f libhblk_blockchain.a

re: fclean all