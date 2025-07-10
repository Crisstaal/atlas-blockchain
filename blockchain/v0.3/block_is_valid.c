#include "blockchain.h"
#include "transaction.h"
#include <string.h>

/**
 * block_is_valid - Validates a block against its previous block
 * @block: Pointer to the block to validate
 * @prev_block: Pointer to the previous block in the blockchain
 * @all_unspent: List of all unspent transaction outputs
 *
 * Return: 0 if valid, or error code:
 *   1 - block or prev_block is NULL when needed
 *   2 - index is not prev_block index + 1
 *   4 - previous hash does not match
 *   5 - block hash mismatch
 *   6 - hash does not match difficulty
 *   7 - invalid transaction list (missing or empty)
 *   8 - first transaction is not valid coinbase
 *   9 - a regular transaction is invalid
 */
int block_is_valid(block_t const *block, block_t const *prev_block,
		   llist_t *all_unspent)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];
	transaction_t *tx;
	int i, tx_count;

	if (!block)
		return (1);

	if (block->info.index != 0)
	{
		if (!prev_block)
			return (1);
		if (block->info.index != prev_block->info.index + 1)
			return (2);
		if (!block->info.prev_hash ||
		    memcmp(block->info.prev_hash, prev_block->hash,
			   SHA256_DIGEST_LENGTH) != 0)
			return (4);
	}

	if (!block_hash(block, hash_buf))
		return (5);

	if (memcmp(block->hash, hash_buf, SHA256_DIGEST_LENGTH) != 0)
		return (5);

	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (6);

	/* Check transaction list */
	tx_count = llist_size(block->transactions);
	if (tx_count < 1)
		return (7);

	tx = llist_get_node_at(block->transactions, 0);
	if (!coinbase_is_valid(tx, block->info.index))
		return (8);

	/* Validate remaining transactions */
	for (i = 1; i < tx_count; i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (!transaction_is_valid(tx, all_unspent))
			return (9);
	}

	return (0);
}
