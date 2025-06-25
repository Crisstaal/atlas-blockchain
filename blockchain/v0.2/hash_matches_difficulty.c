#include "blockchain.h"

/**
 * hash_matches_difficulty - Checks if a hash matches a given difficulty
 * @hash: The hash to check
 * @difficulty: Number of leading zero bits required
 *
 * Return: 1 if difficulty is matched, 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
			    uint32_t difficulty)
{
	uint32_t bits_checked = 0, i;
	uint8_t mask, byte;

	for (i = 0; i < SHA256_DIGEST_LENGTH && bits_checked < difficulty; i++)
	{
		byte = hash[i];

		for (int bit = 7; bit >= 0 && bits_checked < difficulty; bit--, bits_checked++)
		{
			if ((byte >> bit) & 1)
				return (0); /* Found a '1' before reaching required difficulty */
		}
	}

	return (1);
}
