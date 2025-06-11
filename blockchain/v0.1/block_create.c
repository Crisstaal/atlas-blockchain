#include "blockchain.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * block_create - creates a new block
 * @prev: pointer to the previous block
 * @data: data to store in the new block
 * @data_len: length of the data
 * Return: pointer to the created block, or NULL on failure
 */
block_t *block_create(block_t const *prev,
		      int8_t const *data, uint32_t data_len)
{
	block_t *block;

	if (!prev || (!data && data_len))
		return (NULL);

	block = malloc(sizeof(*block));
	if (!block)
		return (NULL);

	/* Initialize metadata */
	block->info.index = prev->info.index + 1;
	block->info.difficulty = 0;
	block->info.timestamp = (uint64_t)time(NULL);
	block->info.nonce = 0;
	memcpy(block->info.prev_hash, prev->hash, SHA256_DIGEST_LENGTH);

	/* Limit data_len and copy data */
	if (data_len > BLOCKCHAIN_DATA_MAX)
		data_len = BLOCKCHAIN_DATA_MAX;
	memcpy(block->data.buffer, data, data_len);
	block->data.len = data_len;

	/* Zero out unused space and hash */
	if (data_len < BLOCKCHAIN_DATA_MAX)
		memset(block->data.buffer + data_len, 0, BLOCKCHAIN_DATA_MAX - data_len);
	memset(block->hash, 0, SHA256_DIGEST_LENGTH);

	return (block);
}
