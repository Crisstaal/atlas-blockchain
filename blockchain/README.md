# Simple Blockchain Project in C

## Overview

This project implements a very basic Blockchain system written in C.

A **Blockchain** is a chain of blocks where each block contains some data and a cryptographic hash linking it to the previous block. This makes the chain secure and tamper-resistant.

---

## Project Structure

- `blockchain.h`  
  Header file containing the definitions of data structures and functions.

- `blockchain_create.c`  
  Functions to create a new blockchain.

- `blockchain_destroy.c`  
  Functions to free memory used by the blockchain.

- `block_create.c`  
  Functions to create new blocks.

- `block_destroy.c`  
  Functions to free memory used by blocks.

- `block_hash.c`  
  Functions to calculate cryptographic hashes for blocks.

- `block_is_valid.c`  
  Functions to check if blocks are valid according to blockchain rules.

- `test/block_is_valid-main.c`  
  A test program to verify the `block_is_valid` function works correctly.

---

## How It Works

- The first block is called the **Genesis Block** and is created manually with fixed data.
- Each new block stores:
  - An index number.
  - The hash of the previous block.
  - Its own data.
  - Its own hash.
- When a block is added:
  - The program verifies that it follows all rules, including proper indexing and hash matching.
  - If the block is valid, it gets added to the chain.
- This ensures the chain is secure and no block is out of order or tampered with.

---

## Requirements

- C compiler (like `gcc`)
- OpenSSL libraries for cryptographic functions
- `llist` library for linked lists

---