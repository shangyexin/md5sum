#include<stdio.h>
#include"md5.h"

#define CHECKFILE "check.md5"

int main(int argc, const char *argv[])
{
	int i;

	if(argc < 2){
		printf("usage: %s [filename]\n",argv[0]);
		return -1;
	}

	if(argc >= 2){
		for(i = 1; i < argc; i++){
			checkmd5(argv[i],CHECKFILE);
		}
	}
	
	
	return 0;
}
