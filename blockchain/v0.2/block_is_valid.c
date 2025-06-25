#include "blockchain.h"

/**
 * block_is_valid - Validates a block against its previous block
 * @block: Pointer to the block to validate
 * @prev_block: Pointer to the previous block in the blockchain
 *
 * Return: 0 if block is valid, otherwise an error code:
 *   1 - block or prev_block is NULL when needed
 *   2 - index is not prev_block index + 1
 *   4 - previous hash does not match
 *   5 - block hash mismatch
 *   6 - block hash does not match difficulty
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];

	if (!block)
		return (1);

	/* For non-genesis blocks, check prev_block */
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

	/* Compute hash of current block */
	if (!block_hash(block, hash_buf))
		return (5);

	/* Compare computed hash with stored hash */
	if (memcmp(block->hash, hash_buf, SHA256_DIGEST_LENGTH) != 0)
		return (5);

	/* Check if hash matches the difficulty */
	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (6);

	return (0);
}
