#include "hblk_crypto.h"
#include <openssl/ecdsa.h>
#include <stdlib.h>

/**
 * ec_verify - Verifies the signature of a given set of bytes using a given EC_KEY public key
 * @key: pointer to EC_KEY structure containing the public key
 * @msg: pointer to the message bytes to verify
 * @msglen: length of the message in bytes
 * @sig: pointer to the signature structure holding the signature data
 *
 * Return: 1 if the signature is valid, 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t const *sig)
{
	if (!key || !msg || !sig || !sig->sig || sig->len == 0)
		return (0);

	/* ECDSA_verify returns 1 for valid, 0 for invalid, -1 for error */
	int verify_status = ECDSA_verify(0, msg, msglen, sig->sig, (int)sig->len, (EC_KEY *)key);

	if (verify_status == 1)
		return (1);
	return (0);
}
