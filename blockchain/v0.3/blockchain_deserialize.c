#include "blockchain.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

/**
 * read_bytes - reads exactly `len` bytes from fd into buf
 */
static int read_bytes(int fd, void *buf, size_t len)
{
	ssize_t rd, total = 0;

	while (len > 0 && (rd = read(fd, buf, len)) > 0)
	{
		total += rd;
		buf = (char *)buf + rd;
		len -= rd;
	}
	return (rd < 0 || len > 0) ? -1 : 0;
}

/**
 * read_uint32 - read 4 bytes as uint32_t
 */
static int read_uint32(int fd, uint32_t *n)
{
	return read_bytes(fd, n, sizeof(*n));
}

/**
 * read_uint64 - read 8 bytes as uint64_t
 */
static int read_uint64(int fd, uint64_t *n)
{
	return read_bytes(fd, n, sizeof(*n));
}

/**
 * read_buffer - reads an exact-length buffer from file
 */
static int read_buffer(int fd, void *buf, size_t size)
{
	return read_bytes(fd, buf, size);
}

/**
 * deserialize_tx_input - reads a transaction input from file
 */
static tx_in_t *deserialize_tx_input(int fd)
{
	tx_in_t *in = calloc(1, sizeof(*in));
	if (!in)
		return NULL;

	if (read_buffer(fd, in->block_hash, SHA256_DIGEST_LENGTH) < 0 ||
	    read_buffer(fd, in->tx_id, SHA256_DIGEST_LENGTH) < 0 ||
	    read_uint32(fd, &in->tx_out_hash_len) < 0 ||
	    read_buffer(fd, in->sig, sizeof(in->sig)) < 0 ||
	    read_uint32(fd, &in->sig_len) < 0)
	{
		free(in);
		return NULL;
	}
	return in;
}

/**
 * deserialize_tx_output - reads a transaction output from file
 */
static tx_out_t *deserialize_tx_output(int fd)
{
	tx_out_t *out = calloc(1, sizeof(*out));
	if (!out)
		return NULL;

	if (read_uint32(fd, &out->amount) < 0 ||
	    read_buffer(fd, out->pub, EC_PUB_LEN) < 0 ||
	    read_buffer(fd, out->hash, SHA256_DIGEST_LENGTH) < 0)
	{
		free(out);
		return NULL;
	}
	return out;
}

/**
 * deserialize_transaction - reads a transaction from file
 */
static transaction_t *deserialize_transaction(int fd)
{
	transaction_t *tx = calloc(1, sizeof(*tx));
	uint32_t count, i;

	if (!tx)
		return NULL;

	if (read_buffer(fd, tx->id, SHA256_DIGEST_LENGTH) < 0)
		goto fail;

	if (read_uint32(fd, &count) < 0)
		goto fail;
	tx->inputs = llist_create(MT_SUPPORT_TRUE);
	for (i = 0; i < count; i++)
	{
		tx_in_t *in = deserialize_tx_input(fd);
		if (!in || llist_add_node(tx->inputs, in, ADD_NODE_REAR) < 0)
			goto fail;
	}

	if (read_uint32(fd, &count) < 0)
		goto fail;
	tx->outputs = llist_create(MT_SUPPORT_TRUE);
	for (i = 0; i < count; i++)
	{
		tx_out_t *out = deserialize_tx_output(fd);
		if (!out || llist_add_node(tx->outputs, out, ADD_NODE_REAR) < 0)
			goto fail;
	}
	return tx;

fail:
	if (tx->inputs)
		llist_destroy(tx->inputs, 1, free);
	if (tx->outputs)
		llist_destroy(tx->outputs, 1, free);
	free(tx);
	return NULL;
}

/**
 * blockchain_deserialize - rebuilds blockchain from file
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	int fd;
	uint8_t magic[4], version[3];
	uint8_t endian;
	uint32_t blocks, i, txs, j;
	blockchain_t *bc;
	block_t *block;
	block_info_t info;
	block_data_t data;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return NULL;

	if (read_buffer(fd, magic, 4) < 0 || memcmp(magic, "HBLK", 4) != 0 ||
	    read_buffer(fd, version, 3) < 0 ||
	    read_buffer(fd, &endian, 1) < 0 ||
	    read_uint32(fd, &blocks) < 0)
	{
		close(fd);
		return NULL;
	}

	bc = calloc(1, sizeof(*bc));
	if (!bc)
		return close(fd), NULL;

	bc->chain = llist_create(MT_SUPPORT_TRUE);
	for (i = 0; i < blocks; i++)
	{
		if (read_buffer(fd, &info, sizeof(info)) < 0 ||
		    read_uint32(fd, &data.len) < 0 ||
		    read_buffer(fd, data.buffer, data.len) < 0 ||
		    read_buffer(fd, data.hash, SHA256_DIGEST_LENGTH) < 0 ||
		    read_uint32(fd, &txs) < 0)
			goto fail;

		block = calloc(1, sizeof(*block));
		if (!block)
			goto fail;

		memcpy(&block->info, &info, sizeof(info));
		memcpy(&block->data, &data, sizeof(data));

		block->transactions = llist_create(MT_SUPPORT_TRUE);
		for (j = 0; j < txs; j++)
		{
			transaction_t *tx = deserialize_transaction(fd);
			if (!tx || llist_add_node(block->transactions, tx, ADD_NODE_REAR) < 0)
				goto fail;
		}

		if (llist_add_node(bc->chain, block, ADD_NODE_REAR) < 0)
			goto fail;
	}

	close(fd);
	return bc;

fail:
	if (bc)
	{
		if (bc->chain)
			llist_destroy(bc->chain, 1, free);
		free(bc);
	}
	close(fd);
	return NULL;
}
