#include "hblk_crypto.h"

/**
 * ec_from_pub - Creates an EC_KEY structure from a public key buffer
 * @pub: Pointer to the public key buffer (uncompressed format)
 *
 * Return: Pointer to newly created EC_KEY, or NULL on failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *key = NULL;
	EC_GROUP *group = NULL;
	EC_POINT *point = NULL;

	if (!pub)
		return (NULL);

	/* Create new EC_KEY using curve NID_secp256k1 */
	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);

	group = (EC_GROUP *)EC_KEY_get0_group(key);
	point = EC_POINT_new(group);
	if (!point)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	/* Convert octet string to EC_POINT */
	if (!EC_POINT_oct2point(group, point, pub, EC_PUB_LEN, NULL))
	{
		EC_POINT_free(point);
		EC_KEY_free(key);
		return (NULL);
	}

	/* Set the public key point into the key */
	if (!EC_KEY_set_public_key(key, point))
	{
		EC_POINT_free(point);
		EC_KEY_free(key);
		return (NULL);
	}

	EC_POINT_free(point);
	return (key);
}
