#include "blockchain.h"
#include "hblk_crypto.h"
#include "transaction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  /* for close() */
#include <fcntl.h>   /* for open() */
#include <errno.h>

#define HBLK_MAGIC "HBLK"
#define HBLK_VERSION "0.3"
#define HBLK_ENDIANNESS 1 /* little endian */

/**
 * write_uint32 - Write a 4-byte unsigned int (little endian) to fd
 * @fd: File descriptor to write to
 * @val: uint32_t value to write
 *
 * Return: 0 on success, -1 on failure
 */
static int write_uint32(int fd, uint32_t val)
{
	uint8_t buf[4];

	buf[0] = val & 0xFF;
	buf[1] = (val >> 8) & 0xFF;
	buf[2] = (val >> 16) & 0xFF;
	buf[3] = (val >> 24) & 0xFF;

	return (write(fd, buf, 4) == 4 ? 0 : -1);
}

/**
 * write_uint64 - Write an 8-byte unsigned int (little endian) to fd
 * @fd: File descriptor to write to
 * @val: uint64_t value to write
 *
 * Return: 0 on success, -1 on failure
 */
static int write_uint64(int fd, uint64_t val)
{
	uint8_t buf[8];
	int i;

	for (i = 0; i < 8; i++)
		buf[i] = (val >> (8 * i)) & 0xFF;

	return (write(fd, buf, 8) == 8 ? 0 : -1);
}

/**
 * write_bytes - Write len bytes from buf to fd
 * @fd: File descriptor to write to
 * @buf: Buffer containing data to write
 * @len: Number of bytes to write
 *
 * Return: 0 on success, -1 on failure
 */
static int write_bytes(int fd, void const *buf, size_t len)
{
	return (write(fd, buf, len) == (ssize_t)len ? 0 : -1);
}

/**
 * serialize_transaction - Serialize a transaction to fd
 * @fd: File descriptor to write to
 * @tx: Pointer to the transaction to serialize
 *
 * Return: 0 on success, -1 on failure
 */
static int serialize_transaction(int fd, transaction_t const *tx)
{
	uint32_t nb_inputs, nb_outputs;
	node_t *node;
	transaction_input_t *input;
	transaction_output_t *output;

	if (!tx)
		return (-1);

	nb_inputs = llist_size(tx->inputs);
	nb_outputs = llist_size(tx->outputs);

	if (write_bytes(fd, tx->id, SHA256_DIGEST_LENGTH) == -1)
		return (-1);

	if (write_uint32(fd, nb_inputs) == -1)
		return (-1);

	if (write_uint32(fd, nb_outputs) == -1)
		return (-1);

	/* Write each input */
	node = tx->inputs->head;
	while (node)
	{
		input = node->data;
		if (write_bytes(fd, input->block_hash, SHA256_DIGEST_LENGTH) == -1 ||
		    write_bytes(fd, input->tx_id, SHA256_DIGEST_LENGTH) == -1 ||
		    write_bytes(fd, input->tx_out_hash, SHA256_DIGEST_LENGTH) == -1 ||
		    write_bytes(fd, input->sig.sig, 72) == -1 ||
		    write_bytes(fd, &input->sig.len, 1) == -1)
			return (-1);
		node = node->next;
	}

	/* Write each output */
	node = tx->outputs->head;
	while (node)
	{
		output = node->data;
		if (write_uint32(fd, output->amount) == -1 ||
		    write_bytes(fd, output->pub, EC_PUB_LEN) == -1 ||
		    write_bytes(fd, output->hash, SHA256_DIGEST_LENGTH) == -1)
			return (-1);
		node = node->next;
	}

	return (0);
}

/**
 * blockchain_serialize - Serialize a blockchain to a file
 * @blockchain: Pointer to blockchain to serialize
 * @path: Path to file to write serialized data
 *
 * Return: 0 on success, -1 on failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	int fd;
	uint32_t nb_blocks, nb_unspent;
	node_t *node;
	block_t *block;
	uint32_t nb_tx, i;
	node_t *tx_node;
	transaction_t *tx;
	unspent_tx_out_t *uout;

	if (!blockchain || !path)
		return (-1);

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (-1);

	/* Write file header */
	if (write_bytes(fd, HBLK_MAGIC, 4) == -1 ||
	    write_bytes(fd, HBLK_VERSION, 3) == -1)
		goto fail;

	{
		uint8_t endian = HBLK_ENDIANNESS;

		if (write_bytes(fd, &endian, 1) == -1)
			goto fail;
	}

	nb_blocks = llist_size(blockchain->chain);
	nb_unspent = llist_size(blockchain->unspent);

	if (write_uint32(fd, nb_blocks) == -1 ||
	    write_uint32(fd, nb_unspent) == -1)
		goto fail;

	/* Serialize blocks */
	node = blockchain->chain->head;
	while (node)
	{
		block = node->data;

		if (write_uint32(fd, block->info.index) == -1 ||
		    write_uint32(fd, block->info.difficulty) == -1 ||
		    write_uint64(fd, block->info.timestamp) == -1 ||
		    write_uint64(fd, block->info.nonce) == -1 ||
		    write_bytes(fd, block->info.prev_hash, SHA256_DIGEST_LENGTH) == -1 ||
		    write_uint32(fd, block->data.len) == -1 ||
		    write_bytes(fd, block->data.buffer, block->data.len) == -1 ||
		    write_bytes(fd, block->hash, SHA256_DIGEST_LENGTH) == -1)
			goto fail;

		nb_tx = 0;
		if (block->transactions)
			nb_tx = llist_size(block->transactions);

		if (write_uint32(fd, nb_tx) == -1)
			goto fail;

		tx_node = block->transactions ? block->transactions->head : NULL;
		while (tx_node)
		{
			tx = tx_node->data;
			if (serialize_transaction(fd, tx) == -1)
				goto fail;
			tx_node = tx_node->next;
		}
		node = node->next;
	}

	/* Serialize unspent transaction outputs */
	node = blockchain->unspent->head;
	while (node)
	{
		uout = node->data;
		if (write_bytes(fd, uout->block_hash, SHA256_DIGEST_LENGTH) == -1 ||
		    write_bytes(fd, uout->tx_id, SHA256_DIGEST_LENGTH) == -1 ||
		    write_uint32(fd, uout->out.amount) == -1 ||
		    write_bytes(fd, uout->out.pub, EC_PUB_LEN) == -1 ||
		    write_bytes(fd, uout->out.hash, SHA256_DIGEST_LENGTH) == -1)
			goto fail;
		node = node->next;
	}

	close(fd);
	return (0);

fail:
	close(fd);
	return (-1);
}
