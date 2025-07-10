#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "crypto/hblk_crypto.h"

#include "blockchain.h"
#include <stddef.h>
#include <stdint.h>
#include <openssl/ec.h>
#include <openssl/sha.h>

#include "llist.h"

#define COINBASE_AMOUNT 50
#define SHA256_DIGEST_LENGTH 32
#define EC_PUB_LEN 65

typedef struct blockchain_s blockchain_t;

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

/**
 * struct transaction_s - Full transaction structure
 *
 * @id: Hash of this transaction
 * @inputs: List of inputs (tx_in_t *)
 * @outputs: List of outputs (tx_out_t *)
 */
typedef struct transaction_s
{
    uint8_t id[SHA256_DIGEST_LENGTH];
    llist_t *inputs;
    llist_t *outputs;
} transaction_t;

/* Function prototypes - declare each function only once */

unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH],
	tx_out_t const *out);

int blockchain_serialize(blockchain_t const *blockchain, char const *path);
blockchain_t *blockchain_deserialize(char const *path);

tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN]);
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent);
uint8_t *transaction_hash(transaction_t const *transaction, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH], EC_KEY const *sender, llist_t *all_unspent);
transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent);
int transaction_is_valid(transaction_t const *transaction, llist_t *all_unspent);
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index);
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index);
void transaction_destroy(transaction_t *transaction);

llist_t *update_unspent(llist_t *transactions, uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent);

#endif /* TRANSACTION_H */
