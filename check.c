#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h"

#define MD5_LENTH 32

static char buf_temp[2];
static char buf_search[MD5_LENTH+1] ={0};
static char buf_check[MD5_LENTH+1] = {0};

FILE *safe_fopen(const char *filename,const char *mode)
{
	FILE *file;
    file = fopen(filename,mode);
	if (file == NULL) {
        printf ("file \"%s\" cannot be opened.\n", filename);
        exit (1);
	}

	return file;
}

/** 
 * 校验文件md5值，在文件列表中查询文件是否被篡改
 * @param filename   待校验文件名
 * @param md5list   文件MD5值列表 
 * @return 成功返回0，错误返回-1
*/

int checkmd5(const char *filename,const char *md5list)
{
    unsigned char md5[MD5_DIGEST_LENGTH];
    int i,bytes;
    int find = 0;
    FILE *inFile,*listFile;
    MD5_CTX mdContext;
    unsigned char data[1024];
	
    inFile = safe_fopen(filename,"rb");	
	listFile = safe_fopen(md5list,"r");

    MD5_Init(&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update(&mdContext, data, bytes);
    MD5_Final(md5,&mdContext);
	for(i = 0; i < MD5_DIGEST_LENGTH; i++){ 
 		sprintf(buf_temp,"%02x", md5[i]);
 		strncat(buf_search,buf_temp,2);
 	}
 	printf("buf_search:%s\n",buf_search);
			
 	while(NULL != fgets(buf_check,33,listFile)){
 		printf("buf_check:%s\n",buf_check);
 		if(0 == strncmp(buf_search,buf_check,32)){
 			printf("%s: verify success!\n",filename);
 			find = 1;
 			break;
 		}
 	}
 		
 	bzero(buf_search,33);
    fclose (inFile);
    fclose (listFile);
    
    if(1 != find){
 		printf("%s: verify failed!\n",filename);
 		return -1;
 	}
 	else
    	return 0;
}

