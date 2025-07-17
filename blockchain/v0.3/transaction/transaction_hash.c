#include "transaction.h"
#include <stdlib.h>
#include <string.h>

/**
 * transaction_hash - computes transaction ID (SHA-256 hash)
 * @transaction: pointer to the transaction
 * @hash_buf: buffer in which to store the computed hash
 * Return: pointer to hash_buf or NULL on failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
			  uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t inputs_len, outputs_len;
	size_t buf_size, offset = 0, i;
	uint8_t *buf;
	tx_in_t *input;
	tx_out_t *output;

	if (!transaction || !hash_buf)
		return (NULL);

	inputs_len = llist_size(transaction->inputs);
	outputs_len = llist_size(transaction->outputs);

	buf_size = (inputs_len * 3 + outputs_len) * SHA256_DIGEST_LENGTH;

	buf = malloc(buf_size);
	if (!buf)
		return (NULL);

	for (i = 0; i < inputs_len; i++)
	{
		input = llist_get_node_at(transaction->inputs, i);
		if (!input)
		{
			free(buf);
			return (NULL);
		}

		memcpy(buf + offset, input->block_hash, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;

		memcpy(buf + offset, input->tx_id, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;

		memcpy(buf + offset, input->tx_out_hash, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;
	}

	for (i = 0; i < outputs_len; i++)
	{
		output = llist_get_node_at(transaction->outputs, i);
		if (!output)
		{
			free(buf);
			return (NULL);
		}

		memcpy(buf + offset, output->hash, SHA256_DIGEST_LENGTH);
		offset += SHA256_DIGEST_LENGTH;
	}

	sha256((const int8_t *)buf, buf_size, hash_buf);

	free(buf);
	return (hash_buf);
}
