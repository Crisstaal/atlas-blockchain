#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include "hblk_crypto.h"

/**
 * ec_create - Creates a new EC key pair using the secp256k1 curve
 *
 * Return: Pointer to the EC_KEY structure containing the key pair,
 *         or NULL on failure
 */
EC_KEY *ec_create(void)
{
    EC_KEY *key = NULL;

    /* Create a new EC_KEY object with curve secp256k1 */
    key = EC_KEY_new_by_curve_name(EC_CURVE);
    if (!key)
        return (NULL);

    /* Generate the key pair */
    if (EC_KEY_generate_key(key) != 1)
    {
        EC_KEY_free(key);
        return (NULL);
    }

    return (key);
}
