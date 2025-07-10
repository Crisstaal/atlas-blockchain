#include "blockchain.h"
#include <stdlib.h>

/**
 * blockchain_destroy - Frees a Blockchain structure
 * @blockchain: pointer to the blockchain to destroy
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;

	if (blockchain->chain)
		llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);

	/* NEW: Destroy unspent list */
	if (blockchain->unspent)
		llist_destroy(blockchain->unspent, 1, free);

	free(blockchain);
}
