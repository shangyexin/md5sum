//
// Created by shangyexin on 2/7/17.
//
#include <stdio.h>
#include "md5check.h"

#define MD5_LIST    "/check.md5"

int main(int argc, char* agrv[])
{
    int ret = 0;

    /*查询列表中所有文件与记录的MD5值是否匹配*/
    ret = md5_check_list(MD5_LIST,NULL);

    if(!ret)
        printf("[Main] Md5 check complete!\n");
    else
        printf("[Main] Md5 check failed!\n");

    return 0;
}

