#include "blockchain.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define HBLK_MAGIC "HBLK"
#define HBLK_VERSION "0.3"

static int write_bytes(FILE *f, void *ptr, size_t size)
{
	return fwrite(ptr, size, 1, f) == 1 ? 0 : -1;
}

static int write_uint32(FILE *f, uint32_t val)
{
	uint32_t net_val = htonl(val);
	return write_bytes(f, &net_val, sizeof(net_val));
}

static int write_uint64(FILE *f, uint64_t val)
{
	uint64_t net_val = htobe64(val);
	return write_bytes(f, &net_val, sizeof(net_val));
}

static int serialize_tx_in(FILE *f, tx_in_t *in)
{
	if (!in)
		return -1;

	if (fwrite(in->block_hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH ||
	    fwrite(in->tx_id, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH ||
	    fwrite(in->tx_out_hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH ||
	    fwrite(in->sig.sig, 1, sizeof(in->sig.sig), f) != sizeof(in->sig.sig) ||
	    fwrite(&in->sig.len, 1, 1, f) != 1)
		return -1;

	return 0;
}

static int serialize_tx_out(FILE *f, tx_out_t *out)
{
	if (!out)
		return -1;

	if (write_uint32(f, out->amount) < 0 ||
	    fwrite(out->pub, 1, EC_PUB_LEN, f) != EC_PUB_LEN ||
	    fwrite(out->hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH)
		return -1;

	return 0;
}

static int serialize_transaction(FILE *f, transaction_t *tx)
{
	uint32_t n_inputs, n_outputs;
	size_t i;

	if (!tx || !f)
		return -1;

	if (fwrite(tx->id, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH)
		return -1;

	n_inputs = llist_size(tx->inputs);
	n_outputs = llist_size(tx->outputs);

	if (write_uint32(f, n_inputs) < 0 || write_uint32(f, n_outputs) < 0)
		return -1;

	for (i = 0; i < n_inputs; i++)
	{
		tx_in_t *in = llist_get_node_at(tx->inputs, i);
		if (serialize_tx_in(f, in) < 0)
			return -1;
	}

	for (i = 0; i < n_outputs; i++)
	{
		tx_out_t *out = llist_get_node_at(tx->outputs, i);
		if (serialize_tx_out(f, out) < 0)
			return -1;
	}

	return 0;
}

static int serialize_block(FILE *f, block_t *block)
{
	uint32_t nb_tx;
	size_t i;

	if (!block || !f)
		return -1;

	if (write_uint32(f, block->info.index) < 0 ||
	    write_uint32(f, block->info.difficulty) < 0 ||
	    write_uint64(f, block->info.timestamp) < 0 ||
	    write_uint64(f, block->info.nonce) < 0 ||
	    fwrite(block->info.prev_hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH ||
	    write_uint32(f, block->data.len) < 0 ||
	    fwrite(block->data.buffer, 1, block->data.len, f) != block->data.len ||
	    fwrite(block->hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH)
		return -1;

	if (block->transactions == NULL)
		return write_uint32(f, -1);

	nb_tx = llist_size(block->transactions);
	if (write_uint32(f, nb_tx) < 0)
		return -1;

	for (i = 0; i < nb_tx; i++)
	{
		transaction_t *tx = llist_get_node_at(block->transactions, i);
		if (serialize_transaction(f, tx) < 0)
			return -1;
	}

	return 0;
}

static int serialize_unspent(FILE *f, unspent_tx_out_t *utxo)
{
	if (!utxo)
		return -1;

	if (fwrite(utxo->block_hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH ||
	    fwrite(utxo->tx_id, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH ||
	    write_uint32(f, utxo->out.amount) < 0 ||
	    fwrite(utxo->out.pub, 1, EC_PUB_LEN, f) != EC_PUB_LEN ||
	    fwrite(utxo->out.hash, 1, SHA256_DIGEST_LENGTH, f) != SHA256_DIGEST_LENGTH)
		return -1;

	return 0;
}

int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *f;
	uint32_t n_blocks, n_unspent;
	size_t i;

	if (!blockchain || !path)
		return -1;

	f = fopen(path, "wb");
	if (!f)
		return -1;

	fwrite(HBLK_MAGIC, 1, 4, f);
	fwrite(HBLK_VERSION, 1, 3, f);
	fputc(1, f); /* Little endian */

	n_blocks = llist_size(blockchain->chain);
	n_unspent = llist_size(blockchain->unspent);

	if (write_uint32(f, n_blocks) < 0 || write_uint32(f, n_unspent) < 0)
	{
		fclose(f);
		return -1;
	}

	for (i = 0; i < n_blocks; i++)
	{
		block_t *blk = llist_get_node_at(blockchain->chain, i);
		if (serialize_block(f, blk) < 0)
		{
			fclose(f);
			return -1;
		}
	}

	for (i = 0; i < n_unspent; i++)
	{
		unspent_tx_out_t *utxo = llist_get_node_at(blockchain->unspent, i);
		if (serialize_unspent(f, utxo) < 0)
		{
			fclose(f);
			return -1;
		}
	}

	fclose(f);
	return 0;
}
