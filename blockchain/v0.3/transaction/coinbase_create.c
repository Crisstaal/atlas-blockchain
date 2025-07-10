#include "transaction.h"
#include <string.h>

/**
 * coinbase_create - Creates a coinbase transaction
 * @receiver: EC public key of the miner
 * @block_index: block index to ensure unique tx
 *
 * Return: pointer to coinbase transaction or NULL on failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	transaction_t *tx;
	tx_in_t *input;
	tx_out_t *output;
	llist_t *inputs, *outputs;
	uint8_t pub[EC_PUB_LEN];

	if (!receiver)
		return (NULL);

	/* Allocate memory for tx input */
	input = calloc(1, sizeof(*input));
	if (!input)
		return (NULL);
	memcpy(input->tx_out_hash, &block_index, sizeof(block_index));

	/* Create lists */
	inputs = llist_create(MT_SUPPORT_FALSE);
	outputs = llist_create(MT_SUPPORT_FALSE);
	if (!inputs || !outputs || llist_add_node(inputs, input, ADD_NODE_REAR) == -1)
		goto fail;

	/* Create output to receiver */
	ec_to_pub(receiver, pub);
	output = tx_out_create(COINBASE_AMOUNT, pub);
	if (!output || llist_add_node(outputs, output, ADD_NODE_REAR) == -1)
		goto fail;

	/* Create transaction */
	tx = calloc(1, sizeof(*tx));
	if (!tx)
		goto fail;

	tx->inputs = inputs;
	tx->outputs = outputs;
	transaction_hash(tx, tx->id);

	return (tx);

fail:
	free(input);
	free(output);
	llist_destroy(inputs, 1, free);
	llist_destroy(outputs, 1, free);
	return (NULL);
}
