#include "blockchain.h"

/**
 * blockchain_difficulty - Computes the next block's difficulty
 * @blockchain: Pointer to the Blockchain to analyze
 *
 * Return: Difficulty value to be assigned to the next block
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	block_t *last_block, *adjustment_block;
	int blockchain_size;
	time_t actual_time, expected_time;
	uint32_t difficulty;

	if (!blockchain || !blockchain->chain)
		return (0);

	blockchain_size = llist_size(blockchain->chain);
	if (blockchain_size == -1)
		return (0);

	last_block = llist_get_tail(blockchain->chain);
	if (!last_block)
		return (0);

	difficulty = last_block->info.difficulty;

	if (last_block->info.index == 0 ||
	    last_block->info.index % DIFFICULTY_ADJUSTMENT_INTERVAL != 0)
		return (difficulty);

	adjustment_block = llist_get_node_at(blockchain->chain,
					     last_block->info.index - DIFFICULTY_ADJUSTMENT_INTERVAL);
	if (!adjustment_block)
		return (difficulty);

	actual_time = last_block->info.timestamp -
		      adjustment_block->info.timestamp;
	expected_time = BLOCK_GENERATION_INTERVAL * DIFFICULTY_ADJUSTMENT_INTERVAL;

	if (actual_time < (expected_time / 2))
		return (difficulty + 1);
	else if (actual_time > (expected_time * 2))
		return (difficulty > 0 ? difficulty - 1 : 0);

	return (difficulty);
}
