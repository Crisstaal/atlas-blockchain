#include "blockchain.h"
#include <stdlib.h>
#include <string.h>

/**
 * blockchain_create - creates a blockchain with the Genesis Block
 *
 * Return: pointer to the created blockchain, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;
	block_t *genesis_block;

	/* Allocate memory for the blockchain structure */
	blockchain = malloc(sizeof(*blockchain));
	if (!blockchain)
		return (NULL);

	/* Create the chain (linked list) */
	blockchain->chain = llist_create(MT_SUPPORT_TRUE);
	if (!blockchain->chain)
	{
		free(blockchain);
		return (NULL);
	}

	/* Allocate and initialize the Genesis Block */
	genesis_block = malloc(sizeof(*genesis_block));
	if (!genesis_block)
	{
		llist_destroy(blockchain->chain, 1, NULL);
		free(blockchain);
		return (NULL);
	}

	/* Fill in the Genesis Block fields */
	genesis_block->info.index = 0;
	genesis_block->info.difficulty = 0;
	genesis_block->info.timestamp = 1537578000;
	genesis_block->info.nonce = 0;
	memset(genesis_block->info.prev_hash, 0, SHA256_DIGEST_LENGTH);

	memcpy(genesis_block->data.buffer, "Holberton School", 16);
	genesis_block->data.len = 16;

	memcpy(genesis_block->hash,
	       "\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d"
	       "\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03",
	       SHA256_DIGEST_LENGTH);

	/* Append the Genesis Block to the blockchain */
	if (llist_add_node(blockchain->chain, genesis_block, ADD_NODE_REAR) == -1)
	{
		free(genesis_block);
		llist_destroy(blockchain->chain, 1, NULL);
		free(blockchain);
		return (NULL);
	}

	return (blockchain);
}
