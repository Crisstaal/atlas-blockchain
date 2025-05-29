#include "hblk_crypto.h"
#include <openssl/sha.h>

/**
 * sha256 - Computes the SHA-256 hash of a sequence of bytes
 * @s: The data to hash
 * @len: The length of the data
 * @digest: Buffer to store the resulting hash
 *
 * Return: A pointer to digest, or NULL on failure
 */
uint8_t *sha256(int8_t const *s, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!digest)
		return (NULL);

	SHA256((const unsigned char *)s, len, digest);
	return (digest);
}
