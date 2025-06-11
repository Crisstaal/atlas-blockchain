#include "blockchain.h"
#include <string.h>

/**
 * block_hash - Computes the hash of a block
 * @block: Pointer to the block to hash
 * @hash_buf: Buffer to store the hash (must be SHA256_DIGEST_LENGTH in size)
 * Return: Pointer to hash_buf or NULL on failure
 */
uint8_t *block_hash(block_t const *block, uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t len;

	if (!block || !hash_buf)
		return (NULL);

	/* Total length = info + data.buffer (up to data.len) */
	len = sizeof(block->info) + block->data.len;

	/* Hash the concatenated info + data */
	sha256((int8_t const *)block, len, hash_buf);

	return (hash_buf);
}
