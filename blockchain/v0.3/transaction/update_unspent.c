#include "blockchain.h"
#include "transaction.h"
#include <stdlib.h>
#include <string.h>

/**
 * match_unspent_by_idx - Helper to match unspent output by its index
 * @node_data: Pointer to unspent_tx_out_t
 * @arg: Pointer to uint32_t index to match
 *
 * Return: 1 if matches, 0 otherwise
 */
static int match_unspent_by_idx(void *node_data, void *arg)
{
	unspent_tx_out_t *uto = node_data;
	uint32_t idx = *(uint32_t *)arg;

	return (uto->out.idx == idx);
}

/**
 * update_unspent - Updates the list of all unspent transaction outputs
 * @transactions: List of validated transactions from a block
 * @block_hash: Hash of the block that contains these transactions
 * @all_unspent: List of all previously unspent transaction outputs
 *
 * Return: New list of unspent transaction outputs on success, or NULL on failure
 */
llist_t *update_unspent(llist_t *transactions,
			uint8_t block_hash[SHA256_DIGEST_LENGTH],
			llist_t *all_unspent)
{
	llist_t *new_unspent;
	transaction_t *tx;
	tx_out_t *tx_out;
	tx_in_t *tx_in;
	unspent_tx_out_t *uto;
	int i, j, num_tx, num_out, num_in;

	if (!transactions || !block_hash || !all_unspent)
		return (NULL);

	new_unspent = llist_create(MT_SUPPORT_TRUE);
	if (!new_unspent)
		return (NULL);

	/* Copy all_unspent into new_unspent */
	for (i = 0; i < (int)llist_size(all_unspent); i++)
	{
		unspent_tx_out_t *original = llist_get_node_at(all_unspent, i);
		if (!original)
			continue;

		unspent_tx_out_t *copy = malloc(sizeof(*copy));
		if (!copy)
			continue;

		memcpy(copy, original, sizeof(*copy));
		llist_add_node(new_unspent, copy, ADD_NODE_REAR);
	}

	num_tx = llist_size(transactions);

	for (i = 0; i < num_tx; i++)
	{
		tx = llist_get_node_at(transactions, i);
		if (!tx)
			continue;

		/* Remove inputs from unspent */
		num_in = llist_size(tx->inputs);
		for (j = 0; j < num_in; j++)
		{
			tx_in = llist_get_node_at(tx->inputs, j);
			if (!tx_in)
				continue;

			uint32_t idx_to_remove = tx_in->tx_out_idx;
			llist_remove_node(new_unspent, match_unspent_by_idx, &idx_to_remove, 1, free);
		}

		/* Add outputs as new unspent */
		num_out = llist_size(tx->outputs);
		for (j = 0; j < num_out; j++)
		{
			tx_out = llist_get_node_at(tx->outputs, j);
			if (!tx_out)
				continue;

			uto = malloc(sizeof(*uto));
			if (!uto)
				continue;

			memcpy(uto->block_hash, block_hash, SHA256_DIGEST_LENGTH);
			memcpy(uto->tx_id, tx->id, SHA256_DIGEST_LENGTH);
			memcpy(&uto->out, tx_out, sizeof(tx_out_t));
			uto->out.idx = j;

			llist_add_node(new_unspent, uto, ADD_NODE_REAR);
		}
	}

	/* Destroy old unspent list */
	llist_destroy(all_unspent, 1, free);

	return (new_unspent);
}
