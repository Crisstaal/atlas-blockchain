#include "blockchain.h"
#include <stdlib.h>
#include "transaction.h"

/**
 * block_destroy - Frees all memory associated with a block,
 *                 including its transactions list
 * @block: Pointer to the block to destroy
 */
void block_destroy(block_t *block)
{
    if (!block)
        return;

    /* Destroy the transactions list and free each transaction */
    if (block->transactions)
        llist_destroy(block->transactions, 1, (void (*)(void *))transaction_destroy);

    free(block);
}
