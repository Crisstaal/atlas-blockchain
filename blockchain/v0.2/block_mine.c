#include "blockchain.h"

/**
 * block_mine - Mines a block by finding a valid hash
 * @block: Pointer to the block to be mined
 *
 * Description: Increment nonce until a hash matches the difficulty.
 * Timestamp stays fixed to block's original timestamp to ensure consistent mining.
 */
void block_mine(block_t *block)
{
    uint64_t ts;

    if (!block)
        return;

    ts = block->info.timestamp;  /* Save initial timestamp */
    block->info.nonce = 0;

    do {
        block->info.nonce++;
        block->info.timestamp = ts;  /* Keep timestamp fixed during mining */
        block_hash(block, block->hash);
    } while (!hash_matches_difficulty(block->hash, block->info.difficulty));
}
