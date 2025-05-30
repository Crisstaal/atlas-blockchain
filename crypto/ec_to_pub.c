#include "hblk_crypto.h"
#include <string.h>

/**
 * ec_to_pub - Extracts the public key from an EC_KEY opaque structure
 * @key: Pointer to EC_KEY structure containing the key pair
 * @pub: Buffer in which to store the public key (uncompressed)
 *
 * Return: Pointer to @pub on success, or NULL on failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	const EC_POINT *pub_key_point;
	const EC_GROUP *group;

	if (!key || !pub)
		return (NULL);

	group = EC_KEY_get0_group(key);
	pub_key_point = EC_KEY_get0_public_key(key);
	if (!group || !pub_key_point)
		return (NULL);

	/* Convert EC_POINT to uncompressed octet string */
	if (!EC_POINT_point2oct(group, pub_key_point,
				POINT_CONVERSION_UNCOMPRESSED, pub, EC_PUB_LEN, NULL))
		return (NULL);

	return (pub);
}
