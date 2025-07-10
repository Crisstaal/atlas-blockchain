#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <openssl/sha.h>

#include "llist.h"


/* Macros */
#define BLOCKCHAIN_DATA_MAX 1024
#define BLOCK_GENERATION_INTERVAL 1
#define DIFFICULTY_ADJUSTMENT_INTERVAL 5

typedef struct transaction_s transaction_t;


/* === Structures === */

typedef struct block_info_s
{
    uint32_t index;
    uint32_t difficulty;
    uint64_t timestamp;
    uint64_t nonce;
    uint8_t prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;

typedef struct block_data_s
{
    int8_t buffer[BLOCKCHAIN_DATA_MAX];
    uint32_t len;
} block_data_t;

typedef struct block_s
{
    block_info_t info;
    block_data_t data;
    llist_t *transactions;
    uint8_t hash[SHA256_DIGEST_LENGTH];
} block_t;

typedef struct blockchain_s
{
    llist_t *chain;      // List of block_t *
    llist_t *unspent;    // List of unspent_tx_out_t * — **Add this**
} blockchain_t;

/* blockchain_create.c */
blockchain_t *blockchain_create(void);

/* blockchain_destroy.c */
void blockchain_destroy(blockchain_t *blockchain);

/* blockchain_serialize.c */
int blockchain_serialize(blockchain_t const *blockchain, char const *path);

/* blockchain_deserialize.c */
blockchain_t *blockchain_deserialize(char const *path);

uint32_t blockchain_difficulty(blockchain_t const *blockchain);

block_t *block_create(block_t const *prev,
                      int8_t const *data, uint32_t data_len);

void block_destroy(block_t *block);

uint8_t *block_hash(block_t const *block,
                    uint8_t hash_buf[SHA256_DIGEST_LENGTH]);

int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
                            uint32_t difficulty);

void block_mine(block_t *block);

/* Add the missing block validation prototype here so no conflicts */
int block_is_valid(block_t const *block,
                   block_t const *prev_block,
                   llist_t *all_unspent);

#endif /* BLOCKCHAIN_H */
