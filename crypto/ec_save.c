#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

/**
 * ec_save - Saves an EC key pair to disk in PEM format
 * @key: EC_KEY to save
 * @folder: Path to the directory to save key files into
 * Return: 1 on success, 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	FILE *fp;
	char priv_path[256], pub_path[256];

	/* Create the directory if it doesn't exist */
	if (mkdir(folder, 0700) == -1 && errno != EEXIST)
		return (0);

	/* Prepare file paths */
	if (snprintf(priv_path, sizeof(priv_path),
	"%s/%s", folder, "key.pem") >= (int)sizeof(priv_path))
		return (0);
	if (snprintf(pub_path, sizeof(pub_path),
	"%s/%s", folder, "key_pub.pem") >= (int)sizeof(pub_path))
		return (0);

	/* Save private key */
	fp = fopen(priv_path, "w");
	if (!fp)
		return (0);
	if (!PEM_write_ECPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	/* Save public key */
	fp = fopen(pub_path, "w");
	if (!fp)
		return (0);
	if (!PEM_write_EC_PUBKEY(fp, key))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	return (1);
}
