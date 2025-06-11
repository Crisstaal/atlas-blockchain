#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdint.h>
#include <stdlib.h>
#include <llist.h>
#include <openssl/sha.h>

#define BLOCKCHAIN_DATA_MAX 1024

/**
 * struct block_info_s - Metadata about a block
 * @index: Block index in the chain
 * @difficulty: Difficulty of mining this block
 * @timestamp: Time of block creation
 * @nonce: Arbitrary number used in mining
 * @prev_hash: SHA256 hash of previous block
 */
typedef struct block_info_s
{
	uint32_t index;
	uint32_t difficulty;
	uint64_t timestamp;
	uint64_t nonce;
	uint8_t prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;

/**
 * struct block_data_s - Data contained in a block
 * @buffer: Actual data, max BLOCKCHAIN_DATA_MAX bytes
 * @len: Data size
 */
typedef struct block_data_s
{
	int8_t buffer[BLOCKCHAIN_DATA_MAX];
	uint32_t len;
} block_data_t;

/**
 * struct block_s - Defines a block in the blockchain
 * @info: Block metadata
 * @data: Block data
 * @hash: SHA256 hash of the block
 */
typedef struct block_s
{
	block_info_t info;
	block_data_t data;
	uint8_t hash[SHA256_DIGEST_LENGTH];
} block_t;

/**
 * struct blockchain_s - Represents the blockchain
 * @chain: Linked list of blocks
 */
typedef struct blockchain_s
{
	llist_t *chain;
} blockchain_t;

blockchain_t *blockchain_create(void);
block_t *block_create(block_t const *prev, int8_t const *data, uint32_t data_len);
void block_destroy(block_t *block);
void blockchain_destroy(blockchain_t *blockchain);
uint8_t *block_hash(block_t const *block, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
int blockchain_serialize(blockchain_t const *blockchain, char const *path);
blockchain_t *blockchain_deserialize(char const *path);
int block_is_valid(block_t const *block, block_t const *prev_block);

#endif /* BLOCKCHAIN_H */
