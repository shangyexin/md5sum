#include <stdio.h>
#include <stdlib.h>
#include "md5.h"

int checkmd5(char *filename)
{
    unsigned char md5[MD5_DIGEST_LENGTH];
    int i;
    FILE *inFile = fopen (filename, "rb");
    FILE *recordfile;
    
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("file \"%s\" cannot be opened.\n", filename);
        exit (1);
    }

    MD5_Init(&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update(&mdContext, data, bytes);
    MD5_Final(md5,&mdContext);
 	for(i = 0; i < MD5_DIGEST_LENGTH; i++) 
 		printf("%02x", md5[i]);
    printf ("  %s\n", filename);
    fclose (inFile);
    return 0;
}
