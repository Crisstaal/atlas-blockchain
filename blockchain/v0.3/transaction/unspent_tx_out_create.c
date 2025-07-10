#include "transaction.h"
#include "crypto/hblk_crypto.h" /* For SHA256_DIGEST_LENGTH */

#include <stdlib.h>
#include <string.h>

/**
 * unspent_tx_out_create - Creates and initializes an unspent transaction output
 * @block_hash: hash of block containing the referenced tx
 * @tx_id: hash of the transaction containing the output
 * @out: the actual transaction output
 *
 * Return: pointer to new unspent_tx_out_t struct or NULL on failure
 */
unspent_tx_out_t *unspent_tx_out_create(uint8_t block_hash[SHA256_DIGEST_LENGTH],
                                       uint8_t tx_id[SHA256_DIGEST_LENGTH],
                                       tx_out_t const *out)
{
    unspent_tx_out_t *unspent;

    if (!block_hash || !tx_id || !out)
        return NULL;

    unspent = calloc(1, sizeof(unspent_tx_out_t));
    if (!unspent)
        return NULL;

    memcpy(unspent->block_hash, block_hash, SHA256_DIGEST_LENGTH);
    memcpy(unspent->tx_id, tx_id, SHA256_DIGEST_LENGTH);
    memcpy(&(unspent->out), out, sizeof(tx_out_t));

    return unspent;
}
