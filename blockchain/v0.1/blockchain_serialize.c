#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "blockchain.h"
#include "llist.h"


/**
 * swap_endian - Swap endianness of bytes in place
 * @value: Pointer to the value to swap
 * @size: Number of bytes
 */
static void swap_endian(void *value, size_t size)
{
	size_t i;
	uint8_t *ptr = value, tmp;

	for (i = 0; i < size / 2; i++)
	{
		tmp = ptr[i];
		ptr[i] = ptr[size - 1 - i];
		ptr[size - 1 - i] = tmp;
	}
}

/**
 * blockchain_serialize - Serialize a blockchain into a file
 * @blockchain: Pointer to the blockchain to serialize
 * @path: File path to serialize into
 * Return: 0 on success, -1 on failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *file;
	uint8_t endian;
	uint32_t blk_count, i;
	block_t *block;

	if (!blockchain || !path)
		return (-1);

	file = fopen(path, "wb");
	if (!file)
		return (-1);

	/* Write header magic */
	if (fwrite("HBLK", 1, 4, file) != 4)
		goto fail;

	/* Write version */
	if (fwrite(HBLK_VERSION, 1, 3, file) != 3)
		goto fail;

	/* Write endianness */
	endian = _get_endianness();
	if (fwrite(&endian, 1, 1, file) != 1)
		goto fail;

	/* Write number of blocks */
	blk_count = llist_size(blockchain->chain);
	if (endian != _get_endianness())
		swap_endian(&blk_count, sizeof(blk_count));
	if (fwrite(&blk_count, sizeof(blk_count), 1, file) != 1)
		goto fail;

	/* Write each block */
	for (i = 0; i < blk_count; i++)
	{
		uint32_t index, difficulty, data_len;
		uint64_t timestamp, nonce;

		block = llist_get_node_at(blockchain->chain, i);
		if (!block)
			goto fail;

		index = block->info.index;
		difficulty = block->info.difficulty;
		timestamp = block->info.timestamp;
		nonce = block->info.nonce;
		data_len = block->data.len;

		if (endian != _get_endianness())
		{
			swap_endian(&index, sizeof(index));
			swap_endian(&difficulty, sizeof(difficulty));
			swap_endian(&timestamp, sizeof(timestamp));
			swap_endian(&nonce, sizeof(nonce));
			swap_endian(&data_len, sizeof(data_len));
		}

		if (fwrite(&index, sizeof(index), 1, file) != 1)
			goto fail;
		if (fwrite(&difficulty, sizeof(difficulty), 1, file) != 1)
			goto fail;
		if (fwrite(&timestamp, sizeof(timestamp), 1, file) != 1)
			goto fail;
		if (fwrite(&nonce, sizeof(nonce), 1, file) != 1)
			goto fail;
		if (fwrite(block->info.prev_hash, 1, 32, file) != 32)
			goto fail;
		if (fwrite(&data_len, sizeof(data_len), 1, file) != 1)
			goto fail;
		if (fwrite(block->data.buffer, 1, block->data.len, file) != block->data.len)
			goto fail;
		if (fwrite(block->hash, 1, 32, file) != 32)
			goto fail;
	}

	fclose(file);
	return (0);

fail:
	fclose(file);
	return (-1);
}
