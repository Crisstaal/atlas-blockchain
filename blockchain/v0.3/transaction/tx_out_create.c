#include <stdlib.h>
#include <string.h>
#include "transaction.h"
#include "crypto/hblk_crypto.h"

/**
 * tx_out_create - Creates and initializes a new
 * transaction output
 * @amount: Amount of coins to transfer
 * @pub: Receiver’s public key
 *
 * Return: Pointer to the created tx_out_t, or NULL on failure
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

	/* Hash the amount + pub key to produce output hash */
	if (!sha256((int8_t const *)out, sizeof(out->amount) + EC_PUB_LEN, out->hash))
	{
		free(out);
		return (NULL);
	}

	return (out);
}
