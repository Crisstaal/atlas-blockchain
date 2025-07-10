#include "transaction.h"

/**
 * transaction_is_valid - Validates a transaction
 * @transaction: pointer to transaction to validate
 * @all_unspent: list of all unspent outputs in the blockchain
 *
 * Return: 1 if valid, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction, llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	tx_in_t *in;
	unspent_tx_out_t *unspent;
	tx_out_t *out;
	uint32_t input_sum = 0, output_sum = 0;
	int i, j;
	EC_KEY *pub_key;

	if (!transaction || !all_unspent)
		return (0);

	/* Verify transaction hash matches */
	if (!transaction_hash(transaction, hash) ||
	    memcmp(hash, transaction->id, SHA256_DIGEST_LENGTH) != 0)
		return (0);

	/* Validate each input */
	for (i = 0; i < llist_size(transaction->inputs); i++)
	{
		in = llist_get_node_at(transaction->inputs, i);
		unspent = NULL;

		/* Search referenced unspent output */
		for (j = 0; j < llist_size(all_unspent); j++)
		{
			unspent = llist_get_node_at(all_unspent, j);
			if (!unspent)
				continue;

			if (memcmp(in->block_hash, unspent->block_hash, SHA256_DIGEST_LENGTH) == 0 &&
			    memcmp(in->tx_id, unspent->tx_id, SHA256_DIGEST_LENGTH) == 0 &&
			    memcmp(in->tx_out_hash, unspent->out.hash, SHA256_DIGEST_LENGTH) == 0)
				break;
			unspent = NULL;
		}

		if (!unspent)
			return (0);

		/* Verify signature */
		pub_key = ec_from_pub(unspent->out.pub);
		if (!pub_key || !ec_verify(pub_key, transaction->id, SHA256_DIGEST_LENGTH, &in->sig))
		{
			EC_KEY_free(pub_key);
			return (0);
		}
		EC_KEY_free(pub_key);

		input_sum += unspent->out.amount;
	}

	/* Sum all outputs */
	for (i = 0; i < llist_size(transaction->outputs); i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		if (!out)
			return (0);
		output_sum += out->amount;
	}

	return (input_sum == output_sum);
}
