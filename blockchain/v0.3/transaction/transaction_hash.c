#include "transaction.h"
#include <stdlib.h>
#include <string.h>

/**
 * transaction_hash - Computes the ID (SHA-256 hash) of a transaction.
 * @transaction: Pointer to the transaction to hash.
 * @hash_buf: Buffer to store the resulting hash (32 bytes).
 *
 * Return: Pointer to hash_buf or NULL on error.
 */
uint8_t *transaction_hash(transaction_t const *transaction,
			  uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t input_count, output_count, i;
	size_t buffer_size;
	uint8_t *buffer, *ptr;

	if (!transaction || !hash_buf)
		return (NULL);

	input_count = llist_size(transaction->inputs);
	output_count = llist_size(transaction->outputs);
	buffer_size = input_count * (SHA256_DIGEST_LENGTH * 3) +
		      output_count * SHA256_DIGEST_LENGTH;

	buffer = calloc(1, buffer_size);
	if (!buffer)
		return (NULL);

	ptr = buffer;

	/* Add inputs: block_hash, tx_id, tx_out_hash */
	for (i = 0; i < input_count; i++)
	{
		tx_in_t *in = llist_get_node_at(transaction->inputs, i);

		if (!in)
			goto clean;
		memcpy(ptr, in->block_hash, SHA256_DIGEST_LENGTH), ptr += SHA256_DIGEST_LENGTH;
		memcpy(ptr, in->tx_id, SHA256_DIGEST_LENGTH), ptr += SHA256_DIGEST_LENGTH;
		memcpy(ptr, in->tx_out_hash, SHA256_DIGEST_LENGTH), ptr += SHA256_DIGEST_LENGTH;
	}

	/* Add outputs: hash */
	for (i = 0; i < output_count; i++)
	{
		tx_out_t *out = llist_get_node_at(transaction->outputs, i);

		if (!out)
			goto clean;
		memcpy(ptr, out->hash, SHA256_DIGEST_LENGTH), ptr += SHA256_DIGEST_LENGTH;
	}

	/* Compute the hash */
	if (!sha256(buffer, buffer_size, hash_buf))
		goto clean;

	free(buffer);
	return (hash_buf);

clean:
	free(buffer);
	return (NULL);
}
