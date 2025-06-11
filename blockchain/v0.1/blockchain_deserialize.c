#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

#define MAGIC "HBLK0.1"
#define MAGIC_LEN 7

/**
 * read_uint32 - Read a 4-byte little-endian unsigned integer from a file
 * @fd: File pointer
 * @out: Pointer to uint32_t to store the value
 *
 * Return: 0 on success, -1 on failure
 */
static int read_uint32(FILE *fd, uint32_t *out)
{
	unsigned char buf[4];

	if (fread(buf, 1, 4, fd) != 4)
		return (-1);

	*out = (uint32_t)buf[0] |
	       ((uint32_t)buf[1] << 8) |
	       ((uint32_t)buf[2] << 16) |
	       ((uint32_t)buf[3] << 24);

	return (0);
}

/**
 * read_uint64 - Read an 8-byte little-endian unsigned integer from a file
 * @fd: File pointer
 * @out: Pointer to uint64_t to store the value
 *
 * Return: 0 on success, -1 on failure
 */
static int read_uint64(FILE *fd, uint64_t *out)
{
	unsigned char buf[8];
	int i;

	if (fread(buf, 1, 8, fd) != 8)
		return (-1);

	*out = 0;
	for (i = 7; i >= 0; i--)
		*out = (*out << 8) | buf[i];

	return (0);
}

/**
 * read_bytes - Read n bytes from file into buffer
 * @fd: File pointer
 * @buf: Buffer to read into
 * @n: Number of bytes to read
 *
 * Return: 0 on success, -1 on failure
 */
static int read_bytes(FILE *fd, void *buf, size_t n)
{
	if (fread(buf, 1, n, fd) != n)
		return (-1);
	return (0);
}

/**
 * blockchain_deserialize - Deserialize a Blockchain from a file
 * @path: Path to the binary file
 *
 * Return: Pointer to deserialized blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *fd = NULL;
	blockchain_t *blockchain = NULL;
	block_t *block = NULL;
	char magic[MAGIC_LEN];
	uint8_t version;
	uint32_t blocks_count, i;
	int ret;

	if (!path)
		return (NULL);

	fd = fopen(path, "rb");
	if (!fd)
		return (NULL);

	/* Read and validate magic */
	if (fread(magic, 1, MAGIC_LEN, fd) != MAGIC_LEN)
		goto fail;
	if (memcmp(magic, MAGIC, MAGIC_LEN) != 0)
		goto fail;

	/* Read and validate version */
	if (fread(&version, 1, 1, fd) != 1)
		goto fail;
	if (version != 1)
		goto fail;

	/* Create empty blockchain */
	blockchain = blockchain_create();
	if (!blockchain)
		goto fail;

	/* Read blocks count */
	ret = read_uint32(fd, &blocks_count);
	if (ret == -1)
		goto fail;

	for (i = 0; i < blocks_count; i++)
	{
		block = block_deserialize(fd);
		if (!block)
			goto fail;

		if (llist_add_node(blockchain->chain, block, ADD_NODE_REAR) == -1)
		{
			block_destroy(block);
			goto fail;
		}
	}

	fclose(fd);
	return (blockchain);

fail:
	if (blockchain)
		blockchain_destroy(blockchain);
	if (fd)
		fclose(fd);
	return (NULL);
}
