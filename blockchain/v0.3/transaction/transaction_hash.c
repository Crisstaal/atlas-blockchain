#include "transaction.h"
#include <string.h>

/**
 * transaction_hash - Computes the transaction ID (hash)
 * @transaction: pointer to transaction to hash
 * @hash_buf: buffer to store the computed hash
 *
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
			  uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t i, buffer_len = 0, num_inputs, num_outputs;
	uint8_t *buffer, *cursor;
	tx_in_t *in;
	tx_out_t *out;

	if (!transaction || !hash_buf)
		return (NULL);

	num_inputs = llist_size(transaction->inputs);
	num_outputs = llist_size(transaction->outputs);

	buffer_len = num_inputs * (3 * SHA256_DIGEST_LENGTH) +
		     num_outputs * SHA256_DIGEST_LENGTH;

	buffer = calloc(1, buffer_len);
	if (!buffer)
		return (NULL);

	cursor = buffer;

	for (i = 0; i < num_inputs; i++)
	{
		in = llist_get_node_at(transaction->inputs, i);
		if (!in)
			goto cleanup;
		memcpy(cursor, in->block_hash, SHA256_DIGEST_LENGTH);
		cursor += SHA256_DIGEST_LENGTH;
		memcpy(cursor, in->tx_id, SHA256_DIGEST_LENGTH);
		cursor += SHA256_DIGEST_LENGTH;
		memcpy(cursor, in->tx_out_hash, SHA256_DIGEST_LENGTH);
		cursor += SHA256_DIGEST_LENGTH;
	}

	for (i = 0; i < num_outputs; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		if (!out)
			goto cleanup;
		memcpy(cursor, out->hash, SHA256_DIGEST_LENGTH);
		cursor += SHA256_DIGEST_LENGTH;
	}

	sha256(buffer, buffer_len, hash_buf);
	free(buffer);
	return (hash_buf);

cleanup:
	free(buffer);
	return (NULL);
}
