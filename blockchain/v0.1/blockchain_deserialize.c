#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

/**
 * read_block_from_file - Helper to read a block from file into a new block_t
 * @fd: opened file pointer
 *
 * Return: pointer to new block_t or NULL on failure
 */
static block_t *read_block_from_file(FILE *fd)
{
    block_t *block = malloc(sizeof(block_t));
    if (!block)
        return NULL;

    /* Read block_info */
    if (fread(&block->info, sizeof(block_info_t), 1, fd) != 1)
    {
        free(block);
        return NULL;
    }

    /* Read block_data */
    if (fread(&block->data.len, sizeof(block->data.len), 1, fd) != 1)
    {
        free(block);
        return NULL;
    }

    if (block->data.len > BLOCKCHAIN_DATA_MAX)
    {
        /* Data length too large, invalid */
        free(block);
        return NULL;
    }

    if (fread(block->data.buffer, sizeof(int8_t), block->data.len, fd) != block->data.len)
    {
        free(block);
        return NULL;
    }

    /* Read block hash */
    if (fread(block->hash, 1, SHA256_DIGEST_LENGTH, fd) != SHA256_DIGEST_LENGTH)
    {
        free(block);
        return NULL;
    }

    return block;
}

/**
 * blockchain_deserialize - Deserialize blockchain from file
 * @path: path to binary file
 *
 * Return: pointer to blockchain_t or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
    FILE *fd = NULL;
    uint8_t magic[7];
    uint8_t version;
    uint32_t blocks_count;
    blockchain_t *blockchain = NULL;
    block_t *block = NULL;
    int ret;
    uint32_t i;

    if (!path)
        return NULL;

    fd = fopen(path, "rb");
    if (!fd)
        return NULL;

    /* Check magic */
    ret = fread(magic, sizeof(magic), 1, fd);
    if (ret != 1 || memcmp(magic, "HBLK0.1", sizeof(magic)) != 0)
    {
        fclose(fd);
        return NULL;
    }

    /* Check version */
    ret = fread(&version, sizeof(version), 1, fd);
    if (ret != 1 || version != 1)
    {
        fclose(fd);
        return NULL;
    }

    /* Read blocks count */
    ret = fread(&blocks_count, sizeof(blocks_count), 1, fd);
    if (ret != 1)
    {
        fclose(fd);
        return NULL;
    }

    /* Create blockchain */
    blockchain = blockchain_create();
    if (!blockchain)
    {
        fclose(fd);
        return NULL;
    }

    /* Deserialize blocks */
    for (i = 0; i < blocks_count; i++)
    {
        block = read_block_from_file(fd);
        if (!block)
        {
            blockchain_destroy(blockchain);
            fclose(fd);
            return NULL;
        }

        /* Add block to blockchain->chain linked list */
        if (llist_add_node(blockchain->chain, block, ADD_NODE_REAR) == NULL)
        {
            block_destroy(block);
            blockchain_destroy(blockchain);
            fclose(fd);
            return NULL;
        }
    }

    fclose(fd);
    return blockchain;
}
