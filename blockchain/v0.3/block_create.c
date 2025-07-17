#include "blockchain.h"
#include <stdlib.h>
#include <string.h>

/**
 * block_create - Creates a new block, initializing
 *                its transaction list to empty
 * @prev: Pointer to previous block, or NULL if none
 * @data: Pointer to block data buffer
 * @data_len: Length of the data buffer
 *
 * Return: Pointer to the created block, or NULL on failure
 */
block_t *block_create(block_t const *prev,
                      int8_t const *data, uint32_t data_len)
{
    block_t *block = calloc(1, sizeof(*block));
    if (!block)
        return (NULL);

    if (prev)
        block->info.index = prev->info.index + 1;

    if (data_len > BLOCKCHAIN_DATA_MAX)
        data_len = BLOCKCHAIN_DATA_MAX;

    memcpy(block->data.buffer, data, data_len);
    block->data.len = data_len;

    block->transactions = llist_create(MT_SUPPORT_FALSE);
    if (!block->transactions)
    {
        free(block);
        return (NULL);
    }

    return (block);
}
