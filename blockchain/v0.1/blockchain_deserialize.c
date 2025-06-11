#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "blockchain.h"

#define MAGIC "HBLK0.1"
#define MAGIC_LEN 7

block_t *block_deserialize(FILE *fd);  

/**
 * read_uint32 - Read a 4-byte little-endian unsigned integer from a file
 * @fd: File pointer
 * @out: Pointer to uint32_t to store the value
 *
 * Return: 0 on success, -1 on failure
 */
static int read_uint32(FILE *fd, uint32_t *out)
{
    unsigned char buf[4];

    if (fread(buf, 1, 4, fd) != 4)
        return (-1);

    *out = (uint32_t)buf[0] |
           ((uint32_t)buf[1] << 8) |
           ((uint32_t)buf[2] << 16) |
           ((uint32_t)buf[3] << 24);

    return (0);
}



/**
 * blockchain_deserialize - Deserialize a Blockchain from a file
 * @path: Path to the binary file
 *
 * Return: Pointer to deserialized blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
    FILE *fd;
    blockchain_t *blockchain;
    block_t *block;
    char magic[MAGIC_LEN];
    uint8_t version;
    uint32_t blocks_count, i;
    int ret;

    if (!path)
        return (NULL);

    fd = fopen(path, "rb");
    if (!fd)
        return (NULL);

    if (fread(magic, 1, MAGIC_LEN, fd) != MAGIC_LEN)
        goto fail;

    if (memcmp(magic, MAGIC, MAGIC_LEN) != 0)
        goto fail;

    if (fread(&version, 1, 1, fd) != 1)
        goto fail;

    if (version != 1)
        goto fail;

    blockchain = blockchain_create();
    if (!blockchain)
        goto fail;

    ret = read_uint32(fd, &blocks_count);
    if (ret == -1)
        goto fail;

    for (i = 0; i < blocks_count; i++)
    {
        block = block_deserialize(fd);
        if (!block)
            goto fail;

        if (llist_add_node(blockchain->chain, block, ADD_NODE_REAR) == -1)
        {
            block_destroy(block);
            goto fail;
        }
    }

    fclose(fd);
    return (blockchain);

fail:
    if (blockchain)
        blockchain_destroy(blockchain);
    if (fd)
        fclose(fd);
    return (NULL);
}
