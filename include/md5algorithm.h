//
// Created by shangyexin on 2/7/17.
//
#ifndef _MD5_ALGORITHM_H_
#define _MD5_ALGORITHM_H_

/* Any 32-bit or wider unsigned integer data type will do */
typedef unsigned int MD5_u32plus;

typedef struct {
    MD5_u32plus lo, hi;
    MD5_u32plus a, b, c, d;
    unsigned char buffer[64];
    MD5_u32plus block[16];
} MD5_CTX;

void md5_init(MD5_CTX *ctx);
void md5_update(MD5_CTX *ctx, const void *data, unsigned long size);
void md5_final(unsigned char *result, MD5_CTX *ctx);

#endif