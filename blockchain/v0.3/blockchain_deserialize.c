#include "blockchain.h"
#include "hblk_crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define HBLK_MAGIC "HBLK"
#define HBLK_VERSION "0.3"
#define HBLK_ENDIANNESS 1

/**
 * read_uint32 - Reads a 4-byte unsigned int from a file descriptor (little endian)
 * @fd: File descriptor to read from
 * @val: Pointer to store the read uint32_t value
 *
 * Return: 0 on success, -1 on failure
 */
static int read_uint32(int fd, uint32_t *val)
{
	uint8_t buf[4];
	int i;

	if (read(fd, buf, 4) != 4)
		return (-1);

	*val = 0;
	for (i = 0; i < 4; i++)
		*val |= (uint32_t)buf[i] << (8 * i);

	return (0);
}

/**
 * read_uint64 - Reads an 8-byte unsigned int from a file descriptor (little endian)
 * @fd: File descriptor to read from
 * @val: Pointer to store the read uint64_t value
 *
 * Return: 0 on success, -1 on failure
 */
static int read_uint64(int fd, uint64_t *val)
{
	uint8_t buf[8];
	int i;

	if (read(fd, buf, 8) != 8)
		return (-1);

	*val = 0;
	for (i = 0; i < 8; i++)
		*val |= (uint64_t)buf[i] << (8 * i);

	return (0);
}

/**
 * read_bytes - Reads len bytes from fd into buf
 * @fd: File descriptor to read from
 * @buf: Buffer to store read bytes
 * @len: Number of bytes to read
 *
 * Return: 0 on success, -1 on failure
 */
static int read_bytes(int fd, void *buf, size_t len)
{
	return (read(fd, buf, len) == (ssize_t)len ? 0 : -1);
}

/**
 * deserialize_tx_input - Deserialize a transaction input from fd
 * @fd: File descriptor to read from
 *
 * Return: Pointer to transaction_input_t on success, NULL on failure
 */
static transaction_input_t *deserialize_tx_input(int fd)
{
	transaction_input_t *tx_in;

	tx_in = malloc(sizeof(*tx_in));
	if (!tx_in)
		return (NULL);

	if (read_bytes(fd, tx_in->block_hash, SHA256_DIGEST_LENGTH) == -1 ||
	    read_bytes(fd, tx_in->tx_id, SHA256_DIGEST_LENGTH) == -1 ||
	    read_bytes(fd, tx_in->tx_out_hash, SHA256_DIGEST_LENGTH) == -1 ||
	    read_bytes(fd, tx_in->sig.sig, 72) == -1 ||
	    read_bytes(fd, &tx_in->sig.len, 1) == -1)
	{
		free(tx_in);
		return (NULL);
	}

	return (tx_in);
}

/**
 * deserialize_tx_output - Deserialize a transaction output from fd
 * @fd: File descriptor to read from
 *
 * Return: Pointer to transaction_output_t on success, NULL on failure
 */
static transaction_output_t *deserialize_tx_output(int fd)
{
	transaction_output_t *tx_out;

	tx_out = malloc(sizeof(*tx_out));
	if (!tx_out)
		return (NULL);

	if (read_uint32(fd, &tx_out->amount) == -1 ||
	    read_bytes(fd, tx_out->pub, EC_PUB_LEN) == -1 ||
	    read_bytes(fd, tx_out->hash, SHA256_DIGEST_LENGTH) == -1)
	{
		free(tx_out);
		return (NULL);
	}

	return (tx_out);
}

/**
 * deserialize_transaction - Deserialize a transaction from fd
 * @fd: File descriptor to read from
 *
 * Return: Pointer to transaction_t on success, NULL on failure
 */
static transaction_t *deserialize_transaction(int fd)
{
	transaction_t *tx;
	uint32_t nb_inputs, nb_outputs;
	uint32_t i;

	tx = malloc(sizeof(*tx));
	if (!tx)
		return (NULL);

	if (read_bytes(fd, tx->id, SHA256_DIGEST_LENGTH) == -1)
		goto fail;

	if (read_uint32(fd, &nb_inputs) == -1)
		goto fail;

	if (read_uint32(fd, &nb_outputs) == -1)
		goto fail;

	tx->inputs = llist_create(MTXFREE);
	tx->outputs = llist_create(MTXFREE);

	if (!tx->inputs || !tx->outputs)
		goto fail;

	for (i = 0; i < nb_inputs; i++)
	{
		transaction_input_t *tx_in = deserialize_tx_input(fd);

		if (!tx_in)
			goto fail;

		llist_add_node(tx->inputs, tx_in, ADD_NODE_REAR);
	}

	for (i = 0; i < nb_outputs; i++)
	{
		transaction_output_t *tx_out = deserialize_tx_output(fd);

		if (!tx_out)
			goto fail;

		llist_add_node(tx->outputs, tx_out, ADD_NODE_REAR);
	}

	return (tx);

fail:
	if (tx)
	{
		if (tx->inputs)
			llist_destroy(tx->inputs, 1);
		if (tx->outputs)
			llist_destroy(tx->outputs, 1);
		free(tx);
	}

	return (NULL);
}
