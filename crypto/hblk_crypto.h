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

/**
 * struct sig_s - Represents a digital signature
 * @sig: Pointer
 * @len: Length
 *
 * This structure is used to store a digital signature and its length.
 */
typedef struct sig_s
{
	uint8_t *sig;
	size_t len;
} sig_t;

/* Function prototypes */
uint8_t *sha256(int8_t const *s, size_t len,
	uint8_t digest[SHA256_DIGEST_LENGTH]);
EC_KEY *ec_create(void);
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);
int ec_save(EC_KEY *key, char const *folder);
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);
int ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t *sig);
int ec_verify(EC_KEY const *key, uint8_t const *msg,
	size_t msglen, sig_t const *sig);

#endif /* HBLK_CRYPTO_H */
