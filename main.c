#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "test_sdl.h"

int main(int argc, char *argv[])
{
    FILE *fp = NULL, *fp2 = NULL;
    char *tempData = NULL;
    int total = 0;

    tempData = (char *)malloc(1024 * 1024);
    fp = fopen("./files_data/test.txt", "rb");              //只读存在打开
    fp2 = fopen("./files_data/touch_out.bin", "wb+"); //读写生成打开
    if (fp != NULL && fp2 != NULL)
    {
        // char str[] = "ssgerhbf";
        // printf("%d\r\n",fwrite(str, 1, sizeof(str) , fp));

        total = fread(tempData, 1, 1024 * 1024, fp);
        printf("%d\r\n", total);
        printf("%d\r\n", fwrite(tempData, 1, total, fp2));

        free(tempData);
        fclose(fp);
        fclose(fp2);
    }
    else
    {
        if (fp == NULL)
            printf("file1 error\r\n");
        if (fp2 == NULL)
            printf("file2 error\r\n");
    }

    test_sdl_framebuffer(0, NULL);
    printf("hello world\r\n");

    return 0;
}
