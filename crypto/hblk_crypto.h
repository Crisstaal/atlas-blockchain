#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

#include <stddef.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/ecdsa.h>

/* Define the elliptic curve to use */
#define EC_CURVE NID_secp256k1

/* Length of the public key in uncompressed form */
#define EC_PUB_LEN 65

/* Signature structure */
typedef struct sig_s
{
    uint8_t *sig;
    size_t len;
} sig_t;

/* Function prototypes */

/**
 * sha256 - Computes SHA256 digest of input string
 * @s: input string
 * @len: length of input string
 * @digest: output buffer (must be SHA256_DIGEST_LENGTH bytes)
 *
 * Return: pointer to digest buffer
 */
uint8_t *sha256(int8_t const *s, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]);

/**
 * ec_create - Creates a new EC key pair on the secp256k1 curve
 *
 * Return: pointer to EC_KEY or NULL on failure
 */
EC_KEY *ec_create(void);

/**
 * ec_to_pub - Extracts the public key from an EC_KEY
 * @key: EC_KEY pointer
 * @pub: buffer to store the public key (must be at least EC_PUB_LEN bytes)
 *
 * Return: pointer to pub buffer on success, NULL on failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);

/**
 * ec_from_pub - Creates an EC_KEY object from a public key buffer
 * @pub: public key buffer (must be EC_PUB_LEN bytes)
 *
 * Return: pointer to EC_KEY or NULL on failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);

/**
 * ec_sign - Signs a message using an EC private key
 * @key: EC_KEY private key
 * @msg: message to sign
 * @msglen: length of message
 * @sig: pointer to signature structure to fill
 *
 * Return: 1 on success, 0 on failure
 */
int ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t *sig);

/**
 * ec_verify - Verifies a signature for a message using EC public key
 * @key: EC_KEY public key
 * @msg: message whose signature is to be verified
 * @msglen: length of message
 * @sig: pointer to signature structure containing signature
 *
 * Return: 1 if signature is valid, 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t const *sig);

#endif /* HBLK_CRYPTO_H */
