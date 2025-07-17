#include "transaction.h"
#include "crypto/hblk_crypto.h"
#include <string.h>
/**
 * coinbase_is_valid - validates a coinbase transaction
 * @coinbase: pointer to the coinbase transaction
 * @block_index: index of the block this coinbase belongs to
 *
 * Return: 1 if valid, 0 otherwise
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	tx_in_t *input;
	tx_out_t *output;
	uint8_t hash[SHA256_DIGEST_LENGTH];

	if (!coinbase ||
	    llist_size(coinbase->inputs) != 1 ||
	    llist_size(coinbase->outputs) != 1)
		return (0);

	input = llist_get_node_at(coinbase->inputs, 0);
	output = llist_get_node_at(coinbase->outputs, 0);

	if (!input || !output)
		return (0);

	/* Check input fields are zeroed and tx_out_hash starts with block_index */
	if (memcmp(input->block_hash, (uint8_t[SHA256_DIGEST_LENGTH]){0}, SHA256_DIGEST_LENGTH) != 0 ||
	    memcmp(input->tx_id, (uint8_t[SHA256_DIGEST_LENGTH]){0}, SHA256_DIGEST_LENGTH) != 0 ||
	    input->sig.len != 0 ||
	    memcmp(input->sig.sig, (uint8_t[SIG_MAX_LEN]){0}, SIG_MAX_LEN) != 0 ||
	    memcmp(input->tx_out_hash, &block_index, sizeof(block_index)) != 0)
		return (0);

	/* Amount must be exactly the coinbase reward */
	if (output->amount != COINBASE_AMOUNT)
		return (0);

	/* Validate transaction ID matches computed hash */
	transaction_hash(coinbase, hash);
	return (memcmp(hash, coinbase->id, SHA256_DIGEST_LENGTH) == 0);
}
