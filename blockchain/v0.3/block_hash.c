#include "blockchain.h"
#include "hblk_crypto.h"
#include <stdlib.h>
#include <string.h>

/**
 * block_hash - Computes the hash of a block including its transactions
 * @block: Pointer to the block to hash
 * @hash_buf: Buffer where the hash will be stored (SHA256_DIGEST_LENGTH)
 *
 * Return: Pointer to hash_buf on success, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX ctx;
	int i, tx_count;
	transaction_t *tx;

	if (!block || !hash_buf)
		return (NULL);

	SHA256_Init(&ctx);

	/* Hash block info */
	SHA256_Update(&ctx, &block->info, sizeof(block_info_t));

	/* Hash block data (only the used length) */
	SHA256_Update(&ctx, &block->data.len, sizeof(uint32_t));
	SHA256_Update(&ctx, block->data.buffer, block->data.len);

	/* Hash all transaction IDs */
	tx_count = llist_size(block->transactions);
	for (i = 0; i < tx_count; i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (!tx)
			continue;

		/* Each tx already has a computed hash ID */
		SHA256_Update(&ctx, tx->id, SHA256_DIGEST_LENGTH);
	}

	SHA256_Final(hash_buf, &ctx);
	return (hash_buf);
}
