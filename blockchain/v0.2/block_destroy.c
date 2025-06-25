#include "blockchain.h"
#include <stdlib.h>

/**
 * block_destroy - frees a block
 * @block: pointer to the block to destroy
 */
void block_destroy(block_t *block)
{
	if (block)
		free(block);
}
