#include <stdlib.h>
#include <string.h>
#include "transaction.h"

/**
 * unspent_tx_out_create - creates an unspent transaction output
 * @block_hash: hash of the Block where the transaction output is located
 * @tx_id: ID (hash) of the transaction
 * @out: pointer to the referenced transaction output
 *
 * Return: pointer to a newly allocated unspent_tx_out_t, or NULL on failure
 */
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH],
	tx_out_t const *out)
{
	unspent_tx_out_t *unspent;

	if (!block_hash || !tx_id || !out)
		return (NULL);

	unspent = malloc(sizeof(unspent_tx_out_t));
	if (!unspent)
		return (NULL);

	memcpy(unspent->block_hash, block_hash, SHA256_DIGEST_LENGTH);
	memcpy(unspent->tx_id, tx_id, SHA256_DIGEST_LENGTH);
	memcpy(&unspent->out, out, sizeof(tx_out_t));

	return (unspent);
}
