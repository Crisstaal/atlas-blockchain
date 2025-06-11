#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH] = {0};

	if (!block)
		return (1);

	/* Check data length */
	if (block->data.len > BLOCKCHAIN_DATA_MAX)
		return (1);

	if (!prev_block)
	{
		/* Must be genesis block */
		if (block->info.index != 0)
			return (1);

		/* Check if block matches the genesis block */
		if (memcmp(&block->info, &genesis_block.info, sizeof(block_info_t)) != 0)
			return (1);

		if (block->data.len != genesis_block.data.len)
			return (1);

		if (memcmp(block->data.buffer, genesis_block.data.buffer, block->data.len) != 0)
			return (1);

		/* Check hash */
		block_hash(block, hash);
		if (memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
			return (1);

		return (0);
	}

	/* For non-genesis blocks */

	/* Check index */
	if (block->info.index != prev_block->info.index + 1)
		return (1);

	/* Check prev_block's hash */
	block_hash(prev_block, hash);
	if (memcmp(hash, prev_block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);

	/* Check prev_hash matches prev_block's hash */
	if (memcmp(block->info.prev_hash, prev_block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);

	/* Check current block's hash */
	block_hash(block, hash);
	if (memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);

	return (0);
}
