#include "blockchain.h"

/**
 * block_mine - Mines a block by finding a valid hash
 * @block: Pointer to the block to be mined
 *
 * Description: Increment nonce and update timestamp until
 * a hash is found that matches the block's difficulty.
 */
void block_mine(block_t *block)
{
	if (!block)
		return;

	block->info.nonce = 0;

	while (1)
	{
		block->info.timestamp = (uint64_t)time(NULL);

		if (!block_hash(block, block->hash))
			return; /* Hashing failed, exit early */

		if (hash_matches_difficulty(block->hash, block->info.difficulty))
			break; /* Found a valid hash */

		block->info.nonce++;
	}
}
