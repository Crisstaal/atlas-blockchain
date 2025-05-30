#include "hblk_crypto.h"
#include <openssl/ecdsa.h>
#include <string.h>
#include <stdlib.h>

/**
 * ec_sign - Signs a given set of bytes using a given EC_KEY private key
 * @key: pointer to EC_KEY structure containing the private key
 * @msg: pointer to the characters to be signed
 * @msglen: length of the message to sign
 * @sig: pointer to the signature structure to fill
 *
 * Return: pointer to the signature buffer on success, or NULL on failure
 */
uint8_t *ec_sign(EC_KEY const *key,
	uint8_t const *msg, size_t msglen, sig_t *sig)
{
	unsigned int sig_len = 0;

	if (!key || !msg || !sig)
		return (NULL);

	/* Allocate a buffer large enough to hold a DER-encoded ECDSA signature */
	sig->sig = calloc(ECDSA_size(key), sizeof(uint8_t));
	if (!sig->sig)
		return (NULL);

	/* Sign the message */
	if (!ECDSA_sign(0, msg, msglen, sig->sig, &sig_len, (EC_KEY *)key))
	{
		free(sig->sig);
		sig->sig = NULL;
		return (NULL);
	}

	sig->len = sig_len;
	return (sig->sig);
}
