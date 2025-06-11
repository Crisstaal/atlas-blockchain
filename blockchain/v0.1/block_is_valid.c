#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

/* Define the genesis block for validation */
static const block_t genesis_block = {
.info = {
	.index = 0,
	.difficulty = 0,
	.timestamp = 1537578000,
	.nonce = 0,
	.prev_hash = {0},
},
.data = {
	.buffer = "Holberton School",
	.len = 16,
},
.hash = {0xc5, 0x2c, 0x26, 0xc8,
	0xb5, 0x46, 0x16, 0x39, 0x63,
	0x5d, 0x8e, 0xdf, 0x2a, 0x97,
	0xd4, 0x8d, 0x0c, 0x8e, 0x00,
	0x09, 0xc8, 0x17, 0xf2, 0xb1,
	0xd3, 0xd7, 0xff, 0x2f, 0x04,
	0x51, 0x58, 0x03}};


/**
 * block_is_valid - Checks if a given block is valid in the blockchain
 * @block: Pointer to the block to validate
 * @prev_block: Pointer to the previous block in the blockchain, or NULL if @block is the genesis block
 *
 * Return: 0 if the block is valid, 1 otherwise
 **/

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

		if (memcmp(block->data.buffer, genesis_block.data.buffer,
			block->data.len) != 0)
			return (1);

		/* Check hash */
		if (memcmp(block->hash, genesis_block.hash, SHA256_DIGEST_LENGTH) != 0)
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
	if (memcmp(block->info.prev_hash, prev_block->hash,
		SHA256_DIGEST_LENGTH) != 0)
		return (1);

	/* Check current block's hash */
	block_hash(block, hash);
	if (memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);

	return (0);
}
