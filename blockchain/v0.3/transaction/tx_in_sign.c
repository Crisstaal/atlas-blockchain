#include "transaction.h"
#include <string.h>

/**
 * tx_in_sign - Signs a transaction input using the sender's private key
 * @in: pointer to the transaction input to sign
 * @tx_id: ID of the transaction containing the input
 * @sender: sender's EC_KEY (private key)
 * @all_unspent: list of all unspent outputs in the blockchain
 *
 * Return: pointer to the input's signature or NULL on failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
		  EC_KEY const *sender, llist_t *all_unspent)
{
	unspent_tx_out_t *unspent;
	uint8_t pub[EC_PUB_LEN];
	int i;

	if (!in || !tx_id || !sender || !all_unspent)
		return (NULL);

	/* Find the matching unspent output */
	for (i = 0; i < llist_size(all_unspent); i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!unspent)
			continue;

		if (memcmp(unspent->block_hash, in->block_hash, SHA256_DIGEST_LENGTH) == 0 &&
		    memcmp(unspent->tx_id, in->tx_id, SHA256_DIGEST_LENGTH) == 0 &&
		    memcmp(unspent->out.hash, in->tx_out_hash, SHA256_DIGEST_LENGTH) == 0)
		{
			ec_to_pub(sender, pub);
			if (memcmp(pub, unspent->out.pub, EC_PUB_LEN) != 0)
				return (NULL); /* Sender doesn't match output owner */

			if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &in->sig))
				return (NULL);

			return (&in->sig);
		}
	}

	return (NULL);
}
