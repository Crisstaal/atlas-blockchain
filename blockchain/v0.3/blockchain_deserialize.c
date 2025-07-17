#include "blockchain.h"
#include "transaction.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * read_transaction - reads a transaction from a file
 * @fd: file descriptor
 *
 * Return: pointer to newly created transaction or NULL on failure
 */
static transaction_t *read_transaction(int fd)
{
	transaction_t *tx;
	int i, nb_inputs, nb_outputs;

	tx = calloc(1, sizeof(*tx));
	if (!tx)
		return (NULL);

	if (read(fd, tx->id, sizeof(tx->id)) != sizeof(tx->id))
		goto fail;

	if (read(fd, &nb_inputs, sizeof(int)) != sizeof(int))
		goto fail;

	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs)
		goto fail;

	for (i = 0; i < nb_inputs; i++)
	{
		tx_in_t in;
		if (read(fd, &in, sizeof(in)) != sizeof(in))
			goto fail;

		if (llist_add_node(tx->inputs, &in, ADD_NODE_REAR) == -1)
			goto fail;
	}

	if (read(fd, &nb_outputs, sizeof(int)) != sizeof(int))
		goto fail;

	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->outputs)
		goto fail;

	for (i = 0; i < nb_outputs; i++)
	{
		tx_out_t out;
		if (read(fd, &out, sizeof(out)) != sizeof(out))
			goto fail;

		if (llist_add_node(tx->outputs, &out, ADD_NODE_REAR) == -1)
			goto fail;
	}

	return (tx);
fail:
	free(tx);
	return (NULL);
}

/**
 * deserialize_block - reads a single block from file
 * @fd: file descriptor
 *
 * Return: pointer to newly created block or NULL
 */
static block_t *deserialize_block(int fd)
{
	block_t *block;
	uint32_t data_len;
	int i, nb_tx;

	block = calloc(1, sizeof(*block));
	if (!block)
		return (NULL);

	if (read(fd, &block->info, sizeof(block_info_t)) != sizeof(block_info_t))
		goto fail;

	if (read(fd, &data_len, sizeof(uint32_t)) != sizeof(uint32_t))
		goto fail;

	block->data.len = data_len;

	if (read(fd, block->data.buffer, data_len) != (ssize_t)data_len)
		goto fail;

	if (read(fd, block->hash, SHA256_DIGEST_LENGTH) != SHA256_DIGEST_LENGTH)
		goto fail;

	if (read(fd, &nb_tx, sizeof(int)) != sizeof(int))
		goto fail;

	block->transactions = llist_create(MT_SUPPORT_FALSE);
	if (!block->transactions)
		goto fail;

	for (i = 0; i < nb_tx; i++)
	{
		transaction_t *tx = read_transaction(fd);
		if (!tx || llist_add_node(block->transactions, tx, ADD_NODE_REAR) == -1)
			goto fail;
	}

	return (block);
fail:
	free(block);
	return (NULL);
}

/**
 * blockchain_deserialize - loads a blockchain from file
 * @path: path to input file
 *
 * Return: pointer to blockchain or NULL
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	int fd, i;
	uint8_t magic[4], version[3], endianness;
	uint32_t nb_blocks;
	blockchain_t *blockchain;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (NULL);

	if (read(fd, magic, 4) != 4 || memcmp(magic, "HBLK", 4) != 0)
		return (close(fd), NULL);

	if (read(fd, version, 3) != 3 || memcmp(version, "0.3", 3) != 0)
		return (close(fd), NULL);

	if (read(fd, &endianness, 1) != 1)
		return (close(fd), NULL);

	if (read(fd, &nb_blocks, sizeof(nb_blocks)) != sizeof(nb_blocks))
		return (close(fd), NULL);

	blockchain = calloc(1, sizeof(*blockchain));
	if (!blockchain)
		return (close(fd), NULL);

	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->chain)
		return (free(blockchain), close(fd), NULL);

	for (i = 0; i < (int)nb_blocks; i++)
	{
		block_t *block = deserialize_block(fd);
		if (!block || llist_add_node(blockchain->chain, block, ADD_NODE_REAR) == -1)
			return (close(fd), NULL);
	}

	close(fd);
	return (blockchain);
}
