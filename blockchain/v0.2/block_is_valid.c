#include "blockchain.h"

/**
 * block_is_valid - Validates a block against the previous one
 * @block: Pointer to the block to validate
 * @prev_block: Pointer to the previous block in the chain
 *
 * Return: 0 if valid, otherwise an error code:
 * 1 - NULL block or prev_block (if needed)
 * 2 - Index mismatch
 * 4 - Previous hash mismatch
 * 5 - Hash mismatch
 * 6 - Hash does not match difficulty
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];

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

	return (0);
}
