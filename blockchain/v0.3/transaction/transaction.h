#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stddef.h>
#include <stdint.h>
#include <openssl/ec.h>
#include <openssl/sha.h>

#include "llist.h"
#include "blockchain.h"	 /* for tx_in_t, tx_out_t, unspent_tx_out_t */
#include "hblk_crypto.h" /* for EC_PUB_LEN, sig_t */

/**
 * struct tx_out_s - Transaction output
 *
 * @amount: Amount received
 * @pub:    Receiver's public address
 * @hash:   Hash of @amount and @pub. Serves as output ID
 */
typedef struct tx_out_s
{
    uint32_t    amount;
    uint8_t     pub[EC_PUB_LEN];
    uint8_t     hash[SHA256_DIGEST_LENGTH];
} tx_out_t;

/**
 * struct tx_in_s - Transaction input
 *
 * Description: A transaction input always refers to a previous
 * transaction output. The only exception is for a Coinbase transaction, that
 * adds new coins to ciruclation.
 *
 * @block_hash:  Hash of the Block containing the transaction @tx_id
 * @tx_id:       ID of the transaction containing @tx_out_hash
 * @tx_out_hash: Hash of the referenced transaction output
 * @sig:         Signature. Prevents anyone from altering the content of the
 *               transaction. The transaction input is signed by the receiver
 *               of the referenced transaction output, using their private key
 */
typedef struct tx_in_s
{
    uint8_t     block_hash[SHA256_DIGEST_LENGTH];
    uint8_t     tx_id[SHA256_DIGEST_LENGTH];
    uint8_t     tx_out_hash[SHA256_DIGEST_LENGTH];
    sig_t       sig;
} tx_in_t;

/**
 * struct unspent_tx_out_s - Unspent transaction output
 *
 * Description: This structure helps identify transaction outputs that were not
 * used in any transaction input yet, making them "available".
 *
 * @block_hash: Hash of the Block containing the transaction @tx_id
 * @tx_id:      ID of the transaction containing @out
 * @out:        Copy of the referenced transaction output
 */
typedef struct unspent_tx_out_s
{
    uint8_t     block_hash[SHA256_DIGEST_LENGTH];
    uint8_t     tx_id[SHA256_DIGEST_LENGTH];
    tx_out_t    out;
} unspent_tx_out_t;

/**
 * struct tx_data_s - tracks data of transaction
 * Description: holds wallet total and transaction details
 *
 * @pub: the public key (derived from private)
 * @amount_total: the sum of coins available to wallet
 * @needed: the amount needed for this transaction
 * @txt: pointer to the transaction struct
 * @sender: the private key
 * @all_unspent: the pointer to the unspent list
 */
typedef struct tx_data_s
{
	uint8_t pub[EC_PUB_LEN];
	uint32_t amount_total;
	uint32_t needed;
	transaction_t *txt;
	EC_KEY const *sender;
	llist_t *all_unspent;
} tx_data_t;
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

/*Transaction functions */

unspent_tx_out_t *unspent_tx_out_create(
    uint8_t block_hash[SHA256_DIGEST_LENGTH],
    uint8_t tx_id[SHA256_DIGEST_LENGTH],
    tx_out_t const *out);

tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN]);


/* Function prototypes */

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
