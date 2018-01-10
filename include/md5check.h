//
// Created by shangyexin on 1/10/18.
//

#ifndef MD5TEST_MD5CHECK_H
#define MD5TEST_MD5CHECK_H

#define MD5_DIGEST_LENGTH 16
#define MD5_LENTH 32
#define MD5_LIST		"/check.md5"

static char md5ListName[256] = {0};

int md5_check_list(const char *md5list,const char * rootpath);

#endif //MD5TEST_MD5CHECK_H
