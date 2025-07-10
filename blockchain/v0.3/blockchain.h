#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <openssl/sha.h>
#include "crypto/hblk_crypto.h"
#include "llist.h"

/* Macros */
#define BLOCKCHAIN_DATA_MAX 1024
#define BLOCK_GENERATION_INTERVAL 1
#define DIFFICULTY_ADJUSTMENT_INTERVAL 5
#define EC_PUB_LEN 65
#define COINBASE_AMOUNT 50

/* Forward declare transaction_t */
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


/**
 * struct tx_in_s - Transaction input structure
 *
 * @block_hash: Hash of the block containing the referenced output
 * @tx_id: Hash of the transaction containing the referenced output
 * @tx_out_hash: Hash of the output within the transaction
 * @sig: Signature that unlocks the referenced output
 */
typedef struct tx_in_s
{
    uint8_t block_hash[SHA256_DIGEST_LENGTH];
    uint8_t tx_id[SHA256_DIGEST_LENGTH];
    uint8_t tx_out_hash[SHA256_DIGEST_LENGTH];
    sig_t sig;
} tx_in_t;

/**
 * struct tx_out_s - Transaction output structure
 *
 * @amount: Amount of coins to transfer
 * @pub: Receiver’s public key
 * @hash: Hash of this output
 */
typedef struct tx_out_s
{
    uint32_t amount;
    uint8_t pub[EC_PUB_LEN];
    uint8_t hash[SHA256_DIGEST_LENGTH];
} tx_out_t;

/**
 * struct unspent_tx_out_s - Unspent transaction output
 *
 * @block_hash: Hash of the block containing the transaction
 * @tx_id: Hash of the transaction
 * @out: Copy of the transaction output
 */
typedef struct unspent_tx_out_s
{
    uint8_t block_hash[SHA256_DIGEST_LENGTH];
    uint8_t tx_id[SHA256_DIGEST_LENGTH];
    tx_out_t out;
} unspent_tx_out_t;


typedef struct block_s
{
    block_info_t info;
    block_data_t data;
    llist_t *transactions;  /* list of transaction_t * */
    uint8_t hash[SHA256_DIGEST_LENGTH];
} block_t;

typedef struct blockchain_s
{
    llist_t *chain;    /* List of block_t * */
    llist_t *unspent;  /* List of unspent_tx_out_t * */
} blockchain_t;

/* === Blockchain functions === */

blockchain_t *blockchain_create(void);
void blockchain_destroy(blockchain_t *blockchain);
int blockchain_serialize(blockchain_t const *blockchain, char const *path);
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
int block_is_valid(block_t const *block,
                   block_t const *prev_block,
                   llist_t *all_unspent);

/* === Transaction functions === */

unspent_tx_out_t *unspent_tx_out_create(
    uint8_t block_hash[SHA256_DIGEST_LENGTH],
    uint8_t tx_id[SHA256_DIGEST_LENGTH],
    tx_out_t const *out);

tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN]);

/* Add more transaction-related prototypes as needed */

#endif /* BLOCKCHAIN_H */
