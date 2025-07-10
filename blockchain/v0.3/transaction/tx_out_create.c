#include "transaction.h"
#include <string.h>

/**
 * tx_out_create - Creates and initializes a transaction output
 * @amount: amount of the transaction
 * @pub: receiver's public key
 *
 * Return: pointer to new tx_out_t struct or NULL on failure
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	tx_out_t *out;

	if (!pub)
		return (NULL);

	out = calloc(1, sizeof(tx_out_t));
	if (!out)
		return (NULL);

	out->amount = amount;
	memcpy(out->pub, pub, EC_PUB_LEN);

	/* Compute hash of the transaction output */
	sha256((int8_t *)out, sizeof(uint32_t) + EC_PUB_LEN, out->hash);

	return (out);
}
