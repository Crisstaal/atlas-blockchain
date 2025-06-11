#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Define your blockchain and block structs as needed */
typedef struct block_s
{
	uint32_t index;
	uint32_t difficulty;
	uint64_t timestamp;
	uint64_t nonce;
	uint8_t prev_hash[32];
	uint32_t data_len;
	int8_t *data;
	uint8_t hash[32];
	struct block_s *next;
} block_t;

typedef struct blockchain_s
{
	block_t *head;
	size_t length;
} blockchain_t;

/* Helper to get system endianness
 * Returns 1 for little endian, 2 for big endian */
static uint8_t get_endianness(void)
{
	uint16_t test = 0x0001;
	uint8_t *byte = (uint8_t *)&test;
	return (byte[0] == 1) ? 1 : 2;
}

/* Helper to swap endianness in place */
static void swap_endian(void *data, size_t size)
{
	size_t i;
	uint8_t *bytes = (uint8_t *)data;
	for (i = 0; i < size / 2; i++)
	{
		uint8_t tmp = bytes[i];
		bytes[i] = bytes[size - 1 - i];
		bytes[size - 1 - i] = tmp;
	}
}

/* Write a 4-byte unsigned int in little or big endian depending on endianness */
static int write_uint32(FILE *f, uint32_t val, uint8_t file_endian)
{
	if (file_endian != get_endianness())
		swap_endian(&val, sizeof(val));
	return fwrite(&val, sizeof(val), 1, f) == 1 ? 0 : -1;
}

/* Write an 8-byte unsigned int in little or big endian depending on endianness */
static int write_uint64(FILE *f, uint64_t val, uint8_t file_endian)
{
	if (file_endian != get_endianness())
		swap_endian(&val, sizeof(val));
	return fwrite(&val, sizeof(val), 1, f) == 1 ? 0 : -1;
}

/**
 * blockchain_serialize - Serialize a blockchain into a file
 * @blockchain: Pointer to blockchain to serialize
 * @path: File path to write serialization
 *
 * Return: 0 on success, -1 on failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *f;
	uint8_t magic[4] = {'H', 'B', 'L', 'K'};
	char version[3] = {'0', '.', '1'}; /* version X.Y */
	uint8_t endian = 1;		   /* 1 = little endian, 2 = big endian */
	size_t i;
	block_t *block;

	if (!blockchain || !path)
		return -1;

	/* Open file for writing (overwrite if exists) */
	f = fopen(path, "wb");
	if (!f)
		return -1;

	endian = get_endianness();

	/* Write File Header */
	if (fwrite(magic, sizeof(magic), 1, f) != 1)
		goto fail;
	if (fwrite(version, sizeof(version), 1, f) != 1)
		goto fail;
	if (fputc(endian, f) == EOF)
		goto fail;

	/* Number of blocks (4 bytes) */
	uint32_t num_blocks = (uint32_t)blockchain->length;
	if (write_uint32(f, num_blocks, endian) < 0)
		goto fail;

	/* Write blocks */
	block = blockchain->head;
	for (i = 0; i < blockchain->length; i++)
	{
		uint32_t tmp32;
		uint64_t tmp64;

		/* index */
		tmp32 = block->index;
		if (write_uint32(f, tmp32, endian) < 0)
			goto fail;

		/* difficulty */
		tmp32 = block->difficulty;
		if (write_uint32(f, tmp32, endian) < 0)
			goto fail;

		/* timestamp */
		tmp64 = block->timestamp;
		if (write_uint64(f, tmp64, endian) < 0)
			goto fail;

		/* nonce */
		tmp64 = block->nonce;
		if (write_uint64(f, tmp64, endian) < 0)
			goto fail;

		/* prev_hash */
		if (fwrite(block->prev_hash, sizeof(block->prev_hash), 1, f) != 1)
			goto fail;

		/* data_len */
		tmp32 = block->data_len;
		if (write_uint32(f, tmp32, endian) < 0)
			goto fail;

		/* data */
		if (block->data_len > 0)
		{
			if (fwrite(block->data, block->data_len, 1, f) != 1)
				goto fail;
		}

		/* hash */
		if (fwrite(block->hash, sizeof(block->hash), 1, f) != 1)
			goto fail;

		block = block->next;
	}

	fclose(f);
	return 0;

fail:
	fclose(f);
	return -1;
}
