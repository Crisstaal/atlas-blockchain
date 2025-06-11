#include "blockchain.h"
#include "hblk_crypto.h"
#include <stdlib.h>
#include <string.h>

/**
 * block_hash - Computes the hash of a block
 * @block: Pointer to the block to hash
 * @hash_buf: Buffer where the hash will be stored (SHA256_DIGEST_LENGTH)
 *
 * Return: Pointer to hash_buf on success, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t to_hash_size;
	uint8_t *to_hash;

	if (!block || !hash_buf)
		return (NULL);

	/* Size = block_info_t + block_data_t.buffer (len only) */
	to_hash_size = sizeof(block->info) + block->data.len;

	to_hash = malloc(to_hash_size);
	if (!to_hash)
		return (NULL);

	/* Copy info */
	memcpy(to_hash, &block->info, sizeof(block->info));
	/* Copy data buffer (only used length) */
	memcpy(to_hash + sizeof(block->info), block->data.buffer, block->data.len);

	/* Compute SHA256 hash */
	sha256((int8_t const *)to_hash, to_hash_size, hash_buf);

	free(to_hash);
	return (hash_buf);
}
