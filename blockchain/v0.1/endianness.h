#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <stdint.h>

/**
 * enum endianness_s - Represents endianness of the system
 * @LITTLE_ENDIAN: Least significant byte is stored first
 * @BIG_ENDIAN: Most significant byte is stored first
 */
typedef enum endianness_s
{
	LITTLE_ENDIAN = 1,
	BIG_ENDIAN = 2
} endianness_t;

/**
 * _get_endianness - Detects the endianness of the system
 * Return: LITTLE_ENDIAN or BIG_ENDIAN
 */
int _get_endianness(void);

#endif /* ENDIANNESS_H */
