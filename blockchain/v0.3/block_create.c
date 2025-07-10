#include "blockchain.h"
#include <stdlib.h>
#include <string.h>

/**
 * blockchain_create - Creates a Blockchain structure, with Genesis Block
 * Return: pointer to the created blockchain, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;
	block_t *genesis;
	llist_t *chain;

	blockchain = malloc(sizeof(*blockchain));
	if (!blockchain)
		return (NULL);

	chain = llist_create(MT_SUPPORT_FALSE);
	if (!chain)
	{
		free(blockchain);
		return (NULL);
	}

	genesis = calloc(1, sizeof(*genesis));
	if (!genesis)
	{
		llist_destroy(chain, 0, NULL);
		free(blockchain);
		return (NULL);
	}

	memcpy(genesis, &_genesis, sizeof(_genesis));

	if (llist_add_node(chain, genesis, ADD_NODE_REAR) != 0)
	{
		free(genesis);
		llist_destroy(chain, 0, NULL);
		free(blockchain);
		return (NULL);
	}

	blockchain->chain = chain;

	/* NEW: Create unspent list */
	blockchain->unspent = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->unspent)
	{
		blockchain_destroy(blockchain);
		return (NULL);
	}

	/* Add the genesis coinbase UTXO */
	transaction_t *coinbase = coinbase_create(NULL, 0);
	unspent_tx_out_t *utxo = unspent_tx_out_create(genesis->hash,
		coinbase->id, &coinbase->outputs[0]);
	llist_add_node(blockchain->unspent, utxo, ADD_NODE_REAR);
	transaction_destroy(coinbase);

	return (blockchain);
}
