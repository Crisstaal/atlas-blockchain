#include "transaction.h"
#include <string.h>

/**
 * tx_in_create - Creates and initializes a transaction input
 * @unspent: pointer to the unspent transaction output to convert
 *
 * Return: pointer to new tx_in_t struct or NULL on failure
 */
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent)
{
	tx_in_t *in;

	if (!unspent)
		return (NULL);

	in = calloc(1, sizeof(tx_in_t));
	if (!in)
		return (NULL);

	memcpy(in->block_hash, unspent->block_hash, SHA256_DIGEST_LENGTH);
	memcpy(in->tx_id, unspent->tx_id, SHA256_DIGEST_LENGTH);
	memcpy(in->tx_out_hash, unspent->out.hash, SHA256_DIGEST_LENGTH);

	/* Signature already zeroed by calloc */
	return (in);
}
