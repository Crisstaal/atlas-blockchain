#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <sys/stat.h>
#include <openssl/err.h>
#include "hblk_crypto.h"

/**
 * ec_load - loads an EC key pair from disk
 * @folder: path to the folder from which to load the keys
 *
 * Return: pointer to created EC_KEY struct or NULL on failure
 */
EC_KEY *ec_load(char const *folder)
{
	EC_KEY *key = NULL;
	FILE *fp = NULL;
	char priv_path[PATH_MAX] = {0};

	if (!folder)
		return (NULL);

	/* Construct private key path */
	snprintf(priv_path, sizeof(priv_path), "%s/%s", folder, "key.pem");

	/* Open and read the private key */
	fp = fopen(priv_path, "r");
	if (!fp)
		return (NULL);

	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);

	if (!key)
		return (NULL);

	return (key);
}
