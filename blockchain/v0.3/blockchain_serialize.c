#include "blockchain.h"
#include "transaction.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * write_transaction - writes a transaction to a file descriptor
 * @fd: file descriptor
 * @transaction: pointer to transaction to write
 *
 * Return: 1 on success, 0 on failure
 */
static int write_transaction(int fd, transaction_t const *transaction)
{
	int i, nb_inputs, nb_outputs;

	if (!transaction)
		return (0);

	if (write(fd, transaction->id, sizeof(transaction->id)) != sizeof(transaction->id))
		return (0);

	nb_inputs = llist_size(transaction->inputs);
	if (write(fd, &nb_inputs, sizeof(int)) != sizeof(int))
		return (0);

	for (i = 0; i < nb_inputs; i++)
	{
		tx_in_t *in = llist_get_node_at(transaction->inputs, i);
		if (!in || write(fd, in, sizeof(*in)) != sizeof(*in))
			return (0);
	}

	nb_outputs = llist_size(transaction->outputs);
	if (write(fd, &nb_outputs, sizeof(int)) != sizeof(int))
		return (0);

	for (i = 0; i < nb_outputs; i++)
	{
		tx_out_t *out = llist_get_node_at(transaction->outputs, i);
		if (!out || write(fd, out, sizeof(*out)) != sizeof(*out))
			return (0);
	}

	return (1);
}

/**
 * serialize_block - writes a single block and its transactions to file
 * @fd: file descriptor
 * @block: pointer to block
 *
 * Return: 1 on success, 0 on failure
 */
static int serialize_block(int fd, block_t const *block)
{
	int i, nb_tx;

	if (write(fd, &block->info, sizeof(block_info_t)) != sizeof(block_info_t))
		return (0);

	if (write(fd, &block->data.len, sizeof(uint32_t)) != sizeof(uint32_t))
		return (0);

	if (write(fd, block->data.buffer, block->data.len) != (ssize_t)block->data.len)
		return (0);

	if (write(fd, block->hash, SHA256_DIGEST_LENGTH) != SHA256_DIGEST_LENGTH)
		return (0);

	nb_tx = llist_size(block->transactions);
	if (write(fd, &nb_tx, sizeof(int)) != sizeof(int))
		return (0);

	for (i = 0; i < nb_tx; i++)
	{
		transaction_t *tx = llist_get_node_at(block->transactions, i);
		if (!write_transaction(fd, tx))
			return (0);
	}

	return (1);
}

/**
 * blockchain_serialize - serializes a blockchain into a file
 * @path: path to output file
 * @blockchain: pointer to blockchain to serialize
 *
 * Return: 1 on success, 0 on failure
 */
int blockchain_serialize(char const *path, blockchain_t const *blockchain)
{
	int fd, i, nb_blocks;
	const uint8_t magic[4] = {'H', 'B', 'L', 'K'};
	const uint8_t version[3] = {'0', '.', '3'};
	uint8_t endianness = _get_endianness();

	if (!path || !blockchain)
		return (0);

	fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (fd < 0)
		return (0);

	write(fd, magic, 4);
	write(fd, version, 3);
	write(fd, &endianness, 1);

	nb_blocks = llist_size(blockchain->chain);
	write(fd, &nb_blocks, sizeof(uint32_t));

	for (i = 0; i < nb_blocks; i++)
	{
		block_t *block = llist_get_node_at(blockchain->chain, i);
		if (!serialize_block(fd, block))
		{
			close(fd);
			return (0);
		}
	}

	close(fd);
	return (1);
}
