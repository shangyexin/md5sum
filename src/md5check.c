//
// Created by shangyexin on 2/7/17.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "md5algorithm.h"

#define MD5_LENTH           32
#define MSG_LENTH           128
#define MD5_DIGEST_LENGTH   16
#define PATH_LENTH          256

#define RECORD_FILE_PATH    "/media/disk/"      /*检验结果记录文件的创建路径，默认输出到U盘*/
#define RECORD_FILE_PREFIX  "update_"           /*检验结果记录文件前缀*/
#define DEFAULT_ROOT_PATH   "/"                 /*默认目标校验根目录，会与check.md5中的文件名进行拼接*/

static char md5_current[MD5_LENTH+1] = {0};

/**
 * 获取当前系统时间
 * @return 时间字符串
*/
char *get_current_time(void)
{
    time_t timep;
    struct tm *p = NULL;
    char hour[4] = {0};
    char minute[4] = {0};
    char second[4] = {0};
    static char current_time[MSG_LENTH] = {0};
    time(&timep);

    p = localtime(&timep); //取得当地时间
    if( NULL == p ) {
        return NULL;
    }

    if(p->tm_hour < 10) {
        sprintf(hour,"0%d",p->tm_hour);
    }else {
        sprintf(hour,"%d",p->tm_hour);
    }

    if(p->tm_min < 10) {
        sprintf(minute,"0%d",p->tm_min);
    }else {
        sprintf(minute,"%d",p->tm_min);
    }

    if(p->tm_sec < 10) {
        sprintf(second,"0%d",p->tm_sec);
    }else {
        sprintf(second,"%d",p->tm_sec);
    }

    bzero(current_time,sizeof(current_time));
    sprintf(current_time,"%d-%d-%d_%s-%s-%s",(1900+p->tm_year), (1+p->tm_mon), p->tm_mday,
             hour, minute, second );

    return current_time;
}

/**
 * 校验传入的文件名是不是普通文件
 * @param filename   待校验文件名
 * @return 是普通文件返回0，其他返回1，错误返回-1
*/
int is_file(const char *filename)
{
	struct stat buf;

	if (lstat(filename, &buf) == -1) {
		printf("%s:stat error\n",filename);
		return -1;
	}

	/*只校验普通文件和软链接，其他格式文件均不校验*/
 	if(S_IFREG == (buf.st_mode & S_IFMT) || S_IFLNK == (buf.st_mode & S_IFMT) ){
 		return 0;
 	}
 	else{
 		return 1;
 	}

}


/**
 * 校验文件md5值,写入到md5_current中
 * @param filename   待校验文件名
 * @return 校验完成返回md5字符串，校验失败返回NULL
*/
char *md5_check(const char *filename)
{
	unsigned char md5[MD5_DIGEST_LENGTH];
	int i,bytes;
	FILE *inFile;
	MD5_CTX mdContext;
	unsigned char data[1024];
	char buf_temp[2] = {0};

	inFile = fopen(filename,"rb");
	if(NULL == inFile){
		printf("[md5_check]open %s failed!\n",filename);
		exit(-1);
	}

	md5_init(&mdContext);
	while ((bytes = fread (data, 1, 1024, inFile)) != 0){
		md5_update(&mdContext, data, bytes);
	}
	md5_final(md5,&mdContext);
	bzero(md5_current,sizeof(md5_current));

	for(i = 0; i < MD5_DIGEST_LENGTH; i++){
		sprintf(buf_temp,"%02x", md5[i]);
		strncat(md5_current,buf_temp,2);
	}

	return md5_current;
}

/**
 * 校验文件md5值，查询列表中所有文件与记录的MD5值是否匹配
 * @param md5list   文件MD5值列表
 * @param rootpath  根文件系统路径
 * @return 校验完成返回0，错误失败返回-1
*/
int md5_check_list(const char *md5list,const char *rootpath)
{
    int ret = 0;
    int fd_md5;
    int count_success = 0;
    int count_fail = 0;
    FILE *listFile;
    char md5_correct[MD5_LENTH+1] = {0};
    char filename[PATH_LENTH] = {0};
    char entryname[PATH_LENTH] = {0};
    char record_file[PATH_LENTH] = {0};
    char check_success_msg[MSG_LENTH] = {0};
    char check_failed_msg[MSG_LENTH] = {0};
    const char* note = "md5 check failed files:\n";//提示信息
    const char* none_msg = "[File not exist]  ";//文件不存在提示信息
    const char* wrong_msg = "[Md5  not match]  ";//文件md5不匹配提示信息

    /*打开待校验列表文件*/
	listFile = fopen(md5list,"r");
	if(NULL == listFile){
		printf("[md5_check] Open %s failed!\n",md5list);
		return -1;
	}

    /*创建结果记录文件*/
    if( NULL != get_current_time() ) {
        sprintf(record_file,"%s%s%s%s",RECORD_FILE_PATH,RECORD_FILE_PREFIX,get_current_time(),".log");
    }else {
        sprintf(record_file,"%s%s%s%s",RECORD_FILE_PATH,RECORD_FILE_PREFIX,"timenull",".log");
    }

    fd_md5 = open(record_file,O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if( fd_md5 > 0 ){
        /*写入提示信息*/
        write(fd_md5,note,strlen(note));
        write(fd_md5,"\n",1);
    } else {
        printf("[md5_check] Open %s failed! md5 check quit-----------------\n",record_file);
        return -1;
    }

    /*获取正确的md5值*/
    while( fscanf(listFile,"%s",md5_correct) != EOF ){

        /*获取文件名*/
        fscanf(listFile,"%s",entryname );

        /*拼接文件名*/
        if (rootpath == NULL) {
            snprintf(filename, PATH_LENTH, "%s%s", DEFAULT_ROOT_PATH, entryname);
        }
        else {
            snprintf(filename, PATH_LENTH, "%s%s", rootpath, entryname);
        }

        /*检验传入的文件名是否为普通文件，不是的话不校验*/
        ret = is_file(filename);
        if( -1 == ret ){
            write(fd_md5,none_msg,strlen(none_msg));
            write(fd_md5,filename,strlen(filename));
            write(fd_md5,"\n",1);
            count_fail ++;
            continue;
        }else if( 1 == ret ) {
            continue;
        }

        /*校验文件的MD5并与列表中正确的MD5进行比较，一致校验通过，否则失败*/
        if( 0 == strncmp(md5_correct,md5_check(filename),MD5_LENTH) ) {
            //printf("%s md5 check success!\n",filename);
            count_success ++;
        }
        else {
            //printf("%s md5 check failed!\n",filename);
            /*向文件中记录md5校验失败的文件*/
            write(fd_md5,wrong_msg,strlen(none_msg));
            write(fd_md5,filename,strlen(filename));
            write(fd_md5,"\n",1);
            count_fail ++;
        }
    }


    write(fd_md5,"\n",1);
    sprintf(check_failed_msg,"%d%s",count_fail," files update failed!");
    write(fd_md5,check_failed_msg,strlen(check_failed_msg));
    write(fd_md5,"\n",1);

    sprintf(check_success_msg,"%d%s",count_success," files update success!");
    write(fd_md5,check_success_msg,strlen(check_success_msg));
    write(fd_md5,"\n",1);

    close(fd_md5);

    return 0;
}