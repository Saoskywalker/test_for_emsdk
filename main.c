#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "test_sdl.h"
#include <emscripten.h>

int main(int argc, char *argv[])
{
    printf("hello 123\r\n");
    
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
        printf("data: %s\r\n",tempData);
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

    // test_sdl();
    test_sdl_framebuffer(0, NULL);

    /**************************************
    编写wasm的SDL程序和平时的不太一样，
    我们需要在程序内部指定我们的游戏主循环，
    这样Web端才能帮我们更新游戏,
    如使用像平时那样使用while大循环，程序会卡死，
    必须使用emscripten_set_main_loop_arg， 注册循环函数来循环
    ****************************************/
    emscripten_set_main_loop_arg(do_loop, NULL, -1, true);
    
    return 0;
}
