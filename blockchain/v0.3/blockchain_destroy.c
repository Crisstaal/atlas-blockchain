#include "blockchain.h"
#include <stdlib.h>

/**
 * blockchain_destroy - Frees an entire blockchain and all its blocks
 * @blockchain: Pointer to the blockchain to destroy
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;

	/* Destroy all blocks in the chain */
	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);

	/* Free the blockchain structure */
	free(blockchain);
}
