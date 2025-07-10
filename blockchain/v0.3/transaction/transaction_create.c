#include "transaction.h"

#include <string.h>

/**
 * transaction_create - Creates a new transaction
 * @sender: sender’s private key
 * @receiver: receiver’s public key
 * @amount: amount to transfer
 * @all_unspent: list of all unspent outputs
 *
 * Return: pointer to the created transaction or NULL on failure
 */
transaction_t *transaction_create(EC_KEY const *sender, EC_KEY const *receiver,
				  uint32_t amount, llist_t *all_unspent)
{
	transaction_t *tx;
	llist_t *inputs, *outputs;
	uint8_t pub_sender[EC_PUB_LEN], pub_receiver[EC_PUB_LEN];
	uint32_t total = 0, leftover;
	int i;
	tx_out_t *out_send, *out_change;
	tx_in_t *input;
	unspent_tx_out_t *unspent;

	if (!sender || !receiver || !all_unspent)
		return (NULL);

	ec_to_pub(sender, pub_sender);
	ec_to_pub(receiver, pub_receiver);

	inputs = llist_create(MT_SUPPORT_FALSE);
	outputs = llist_create(MT_SUPPORT_FALSE);
	if (!inputs || !outputs)
		return (NULL);

	/* Select unspent outputs owned by sender */
	for (i = 0; i < llist_size(all_unspent) && total < amount; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!unspent)
			continue;

		if (memcmp(unspent->out.pub, pub_sender, EC_PUB_LEN) == 0)
		{
			input = tx_in_create(unspent);
			if (!input || llist_add_node(inputs, input, ADD_NODE_REAR) == -1)
				goto fail;
			total += unspent->out.amount;
		}
	}

	if (total < amount)
		goto fail;

	/* Output to receiver */
	out_send = tx_out_create(amount, pub_receiver);
	if (!out_send || llist_add_node(outputs, out_send, ADD_NODE_REAR) == -1)
		goto fail;

	/* Return change to sender, if needed */
	leftover = total - amount;
	if (leftover > 0)
	{
		out_change = tx_out_create(leftover, pub_sender);
		if (!out_change || llist_add_node(outputs, out_change, ADD_NODE_REAR) == -1)
			goto fail;
	}

	/* Allocate transaction */
	tx = calloc(1, sizeof(*tx));
	if (!tx)
		goto fail;

	tx->inputs = inputs;
	tx->outputs = outputs;
	transaction_hash(tx, tx->id);

	/* Sign each input */
	for (i = 0; i < llist_size(inputs); i++)
	{
		input = llist_get_node_at(inputs, i);
		if (!tx_in_sign(input, tx->id, sender, all_unspent))
		{
			free(tx);
			goto fail;
		}
	}

	return (tx);

fail:
	llist_destroy(inputs, 1, free);
	llist_destroy(outputs, 1, free);
	return (NULL);
}
