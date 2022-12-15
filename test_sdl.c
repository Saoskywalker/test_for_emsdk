#include <stdio.h>
// #include <tchar.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <SDL2/SDL.h>

uint32_t color_set = 0XFFFF0000;

void test_sdl(void)
{
    if (SDL_Init((SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) == -1) //SDL_初始化
    {
        printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
    }
    else
    {
        printf("SDL initialized.\n");
        // SDL_Quit(); //退出SDL调用
    }
} 

/**
 * PCM使用
 * 函数调用步骤如下: 
 *
 * [初始化]
 * SDL_Init(): 初始化SDL。
 * SDL_OpenAudio(): 根据参数（存储于SDL_AudioSpec）打开音频设备。
 * SDL_PauseAudio(): 播放音频数据。
 *
 * [循环播放数据]
 * SDL_Delay(): 延时等待播放完成。
 *
 */
 
//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|
static  uint8_t  *audio_chunk; 
static  uint32_t  audio_len = 0; 
static  uint8_t  *audio_pos; 
 
/* Audio Callback
 * The audio function callback takes the following parameters: 
 * stream: A pointer to the audio buffer to be filled 
 * len: The length (in bytes) of the audio buffer 
 * 
*/ 
void  fill_audio(void *udata, uint8_t *stream, int len){ 
	//SDL 2.0
	SDL_memset(stream, 0, len);
	if(audio_len==0)
			return; 
	len=(len>audio_len?audio_len:len);
 
	SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
	audio_pos += len; 
	audio_len -= len; 
}

int _WAV_Play(char *path);
int test_sdl_audio(int argc, char* argv[])
{
	//Init
	// if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {  
	// 	printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
	// 	return -1;
	// }
 
    _WAV_Play("./files_data/music.wav");
	// SDL_Quit();
 
	return 0;
}

/*wav音频解码*/
unsigned char wav_buff[2][70000] __attribute__((aligned(4)));
struct wav_
{
    uint16_t audio_format;       //格式
    uint16_t num_channels;       //通道数
    uint32_t format_blok_lenght; //格式块长度
    uint32_t sample_rate;        //采集频率
    uint32_t byte_rate;          //数据传输率
    uint16_t bits_per_sample;    //采样位宽度
    uint32_t data_size;          //数据区大小
    uint32_t play_time_ms;       //播放时长
};
static struct wav_ wav;

#define ReadSize (512 * 40) /*一次读取数据大小*/
int i, tj = 1;
int a, b, u, Inx, r = 0;
uint8_t buff[8192];
int res = 0;
FILE *fp1 = NULL;
int data_count = 0;
SDL_AudioSpec wanted_spec;

void sound_run(void)
{
    if (audio_len > 0) // Wait until finish
        return;

    if (a)
        a--;
    u++;
    Inx++;
    if (a >= 1)
    {
        /*output pcm data*/
        data_count += ReadSize;
        // Set audio buffer (PCM data)
        audio_chunk = (uint8_t *)wav_buff[Inx % 2];
        // Audio buffer length
        audio_len = ReadSize;
        audio_pos = audio_chunk;
        /**********************/

        fseek(fp1, i + 4 + ((1 + u) * ReadSize), SEEK_SET);
        if (a == 1) // 读剩余数据
        {
            if ((Inx % 2) == 0)
                fread(wav_buff[1], 1, b, fp1);
            if ((Inx % 2) == 1)
                fread(wav_buff[0], 1, b, fp1);
        }
        else
        {
            if ((Inx % 2) == 0)
                fread(wav_buff[1], 1, ReadSize, fp1);
            if ((Inx % 2) == 1)
                fread(wav_buff[0], 1, ReadSize, fp1);
        }
    }
    else
    {
        if (b > 0)
        {
            /*output pcm data*/
            data_count += b;
            // Set audio buffer (PCM data)
            audio_chunk = (uint8_t *)wav_buff[Inx % 2];
            // Audio buffer length
            audio_len = b;
            audio_pos = audio_chunk;
            /**********************/

            b = 0;
        }
        else
        {
            printf("play end\r\n");
            // break;
        }
    }
}

int _WAV_Play(char *path)
{
    printf("WAV play\r\n");

    if (path == NULL)
    {
        printf("no path input..\r\n");
        return -4;
    }
    fp1 = fopen(path, "rb");
    if (fp1 == NULL)
    {
        res = 4;
    }
    else
    {
        res = 0;
    }
    if (res == 5)
    {
        printf("no path..\r\n");
        return -3;
    }
    if (res == 4)
    {
        printf("WAV file not exist..\r\n");
        return -2;
    }
    if (res == 0)
    {
        printf("WAV file open successful..\r\n");
        color_set = 0XFF00FF00;
        /*解WAV文件*/
        fread(buff, 1, 58, fp1);
        //for(i=0;i<44;i++)printf("%02x ",buff[i]);
        wav.audio_format = buff[21] << 8 | buff[20];
        wav.num_channels = buff[23] << 8 | buff[22];
        wav.bits_per_sample = buff[35] << 8 | buff[34];
        wav.byte_rate = buff[31] << 24 | buff[30] << 16 | buff[29] << 8 | buff[28];
        wav.sample_rate = buff[27] << 24 | buff[26] << 16 | buff[25] << 8 | buff[24];
        wav.format_blok_lenght = buff[19] << 24 | buff[18] << 16 | buff[17] << 8 | buff[16];
        i = 16 + wav.format_blok_lenght + 4 + 4;
        wav.data_size = buff[i + 3] << 24 | buff[i + 2] << 16 | buff[i + 1] << 8 | buff[i + 0];
        wav.play_time_ms = (int)((float)wav.data_size * (float)1000 / (float)wav.byte_rate);

        /*输出*/
        printf("sample format:%d \r\n", wav.audio_format);
        printf("sample channel:%d \r\n", wav.num_channels);
        printf("sample bit:%d bit\r\n", wav.bits_per_sample);
        printf("sample cycle:%d Hz\r\n", wav.sample_rate);
        printf("sample rate:%d Byte/S\r\n", wav.byte_rate);
        printf("format block lenght:%d Byte\r\n", wav.format_blok_lenght);
        printf("data size:%d Byte\r\n", wav.data_size);
        // int wav_play_time = 0; //播放时长
        // wav_play_time = wav.play_time_ms / 1000;
        // printf("play time:%02d:%02d:%02d \r\n",
        //        wav_play_time / 3600, wav_play_time % 3600 / 60, wav_play_time % 3600 % 60);

        /*无压缩格式*/
        if (wav.audio_format == 1)
        {
            a = wav.data_size / ReadSize;
            b = wav.data_size % ReadSize;
            /*读出数据区*/
            if (a == 1)
            {
                fseek(fp1, i + 4, SEEK_SET);
                fread(wav_buff[0], 1, ReadSize, fp1);
                if (b > 0)
                {
                    fseek(fp1, i + 4 + ReadSize, SEEK_SET);
                    fread(wav_buff[1], 1, b, fp1);
                }
                r = ReadSize;
            }
            else if (a >= 2)
            {
                fseek(fp1, i + 4, SEEK_SET);
                fread(wav_buff[0], 1, ReadSize, fp1);
                fseek(fp1, i + 4 + ReadSize, SEEK_SET);
                fread(wav_buff[1], 1, ReadSize, fp1);
                r = ReadSize;
            }
            else if ((a == 0) && (b > 0))
            {
                fseek(fp1, i + 4, SEEK_SET);
                fread(wav_buff[0], 1, b, fp1);
                r = b;
                b = 0;
            }

            /*audio init*/
            //SDL_AudioSpec
            wanted_spec.freq = wav.sample_rate; //44100;
            wanted_spec.channels = wav.num_channels; //2;
            wanted_spec.silence = 0;
            wanted_spec.samples = 1024;
            wanted_spec.callback = fill_audio;
            if (wav.bits_per_sample == 16)
                wanted_spec.format = AUDIO_S16SYS;
            else if (wav.bits_per_sample == 32)
                wanted_spec.format = AUDIO_S32SYS;
            else
                return -6;

            if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
            {
                printf("can't open audio.\n");
                return -1;
            }

            //Play
            SDL_PauseAudio(0);
            /************************/

            /*output pcm data*/
            data_count += r;
            //Set audio buffer (PCM data)
            audio_chunk = (uint8_t *)wav_buff[0];
            //Audio buffer length
            audio_len = r;
            audio_pos = audio_chunk;
            /**********************/

            Inx = 0;
            u = 0;
            // while (1)
            // {
            //     sound_run();
            // }
            return 0;
        }
        else
            printf("format error\r\n");

        printf("file close\r\n");
        fclose(fp1);
    }
    return 0;
}

/**
 *
 * 本程序使用SDL2播放RGB/YUV视频像素数据。SDL实际上是对底层绘图
 * API（Direct3D，OpenGL）的封装，使用起来明显简单于直接调用底层
 * API。
 *
 * 函数调用步骤如下: 
 *
 * [初始化]
 * SDL_Init(): 初始化SDL。
 * SDL_CreateWindow(): 创建窗口（Window）。
 * SDL_CreateRenderer(): 基于窗口创建渲染器（Render）。
 * SDL_CreateTexture(): 创建纹理（Texture）。
 *
 * [循环渲染数据]
 * SDL_UpdateTexture(): 设置纹理的数据。
 * SDL_RenderCopy(): 纹理复制给渲染器。
 * SDL_RenderPresent(): 显示。
 *
 * This software plays RGB/YUV raw video data using SDL2.
 * SDL is a wrapper of low-level API (Direct3D, OpenGL).
 * Use SDL is much easier than directly call these low-level API.  
 *
 * The process is shown as follows:
 *
 * [Init]
 * SDL_Init(): Init SDL.
 * SDL_CreateWindow(): Create a Window.
 * SDL_CreateRenderer(): Create a Render.
 * SDL_CreateTexture(): Create a Texture.
 *
 * [Loop to Render data]
 * SDL_UpdateTexture(): Set Texture's data.
 * SDL_RenderCopy(): Copy Texture to Render.
 * SDL_RenderPresent(): Show.
 */
 
//set '1' to choose a type of file to play
#define LOAD_BGRA    1
#define LOAD_RGB24   0
#define LOAD_BGR24   0
#define LOAD_YUV420P 0
 
//Bit per Pixel
#if LOAD_BGRA
const int bpp=32;
#elif LOAD_RGB24|LOAD_BGR24
const int bpp=24;
#elif LOAD_YUV420P
const int bpp=12;
#endif
 
int screen_w=600,screen_h=800;
const int pixel_w=600,pixel_h=800;
 
//Convert RGB24/BGR24 to RGB32/BGR32
//And change Endian if needed
void CONVERT_24to32(unsigned char *image_in,unsigned char *image_out,int w,int h){
	for(int i =0;i<h;i++)
		for(int j=0;j<w;j++){
			//Big Endian or Small Endian?
			//"ARGB" order:high bit -> low bit.
			//ARGB Format Big Endian (low address save high MSB, here is A) in memory : A|R|G|B
			//ARGB Format Little Endian (low address save low MSB, here is B) in memory : B|G|R|A
			if(SDL_BYTEORDER==SDL_LIL_ENDIAN){
				//Little Endian (x86): R|G|B --> B|G|R|A
				image_out[(i*w+j)*4+0]=image_in[(i*w+j)*3+2];
				image_out[(i*w+j)*4+1]=image_in[(i*w+j)*3+1];
				image_out[(i*w+j)*4+2]=image_in[(i*w+j)*3];
				image_out[(i*w+j)*4+3]='0';
			}else{
				//Big Endian: R|G|B --> A|R|G|B
				image_out[(i*w+j)*4]='0';
				memcpy(image_out+(i*w+j)*4+1,image_in+(i*w+j)*3,3);
			}
		}
}
 
 
//Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
 
int thread_exit=0;
 
int refresh_video(void *opaque){
	while (thread_exit==0) {
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	return 0;
}

unsigned char buffer[pixel_w * pixel_h * bpp / 8];
//BPP=32
unsigned char buffer_convert[pixel_w * pixel_h * 4];
SDL_Renderer *sdlRenderer;
SDL_Window *screen; 
SDL_Texture *sdlTexture;
int test_sdl_framebuffer(int argc, char* argv[])
{
	// if(SDL_Init(SDL_INIT_VIDEO)) {  
	// 	printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
	// 	return -1;
	// } 

	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindow("MTFsss fe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,0);
	if(!screen) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);

//     unsigned char *buffer = malloc(pixel_w * pixel_h * bpp / 8);
//     //BPP=32
//     unsigned char *buffer_convert = malloc(pixel_w * pixel_h * 4);

//     memset(buffer, 0X0000FF00, pixel_w * pixel_h * bpp / 8); 
// memset(buffer_convert, 0X0000FF00, pixel_w * pixel_h * 4);
    // unsigned char buffer[pixel_w * pixel_h * bpp / 8];
    // //BPP=32
    // unsigned char buffer_convert[pixel_w * pixel_h * 4];     
    uint32_t pixformat = 0;
#if LOAD_BGRA
	//Note: ARGB8888 in "Little Endian" system stores as B|G|R|A
	pixformat= SDL_PIXELFORMAT_ARGB8888;  
#elif LOAD_RGB24
	pixformat= SDL_PIXELFORMAT_RGB888;  
#elif LOAD_BGR24
	pixformat= SDL_PIXELFORMAT_BGR888;  
#elif LOAD_YUV420P
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat= SDL_PIXELFORMAT_IYUV;  
#endif
 
	sdlTexture = SDL_CreateTexture(sdlRenderer,pixformat, SDL_TEXTUREACCESS_STREAMING,pixel_w,pixel_h);
    // SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
 
 
	FILE *fp=NULL;
#if LOAD_BGRA
	// fp=fopen("./test_bgra_320x180.rgb","rb+");
#elif LOAD_RGB24
	fp=fopen("../test_rgb24_320x180.rgb","rb+");
#elif LOAD_BGR24
	fp=fopen("../test_bgr24_320x180.rgb","rb+");
#elif LOAD_YUV420P
	fp=fopen("../test_yuv420p_320x180.yuv","rb+");
#endif
	// if(fp==NULL){
	// 	printf("cannot open this file\n");
	// 	return -1;
	// }
 
	SDL_Rect sdlRect;  
 
	SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video,NULL,NULL);
	SDL_Event event;
    static uint32_t color = 0XFFFF0000, *bf_bf = NULL; //ARGB
    memset(buffer, 0XFF, sizeof(buffer));
    bf_bf = (uint32_t *)buffer;
	while(0){
		//Wait
		SDL_WaitEvent(&event);
		if(event.type==REFRESH_EVENT)
        {
			// if (fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp) != pixel_w*pixel_h*bpp/8){
			// 	// Loop
			// 	fseek(fp, 0, SEEK_SET);
			// 	fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp);
			// }

            if (color <= 0XFFFF0000)
                color += 0X00100000;
            else
                color = 0XFFFF0000;

            for (size_t ii = 0; ii < sizeof(buffer) / 4; ii++)
                bf_bf[ii] = color;

#if LOAD_BGRA
			//We don't need to change Endian
			//Because input BGRA pixel data(B|G|R|A) is same as ARGB8888 in Little Endian (B|G|R|A)
			SDL_UpdateTexture( sdlTexture, NULL, buffer, pixel_w*4);  
#elif LOAD_RGB24|LOAD_BGR24
			//change 24bit to 32 bit
			//and in Windows we need to change Endian
			CONVERT_24to32(buffer,buffer_convert,pixel_w,pixel_h);
			SDL_UpdateTexture( sdlTexture, NULL, buffer_convert, pixel_w*4);  
#elif LOAD_YUV420P
			SDL_UpdateTexture( sdlTexture, NULL, buffer, pixel_w);  
#endif
			//FIX: If window is resize
			sdlRect.x = 0;  
			sdlRect.y = 0;  
			sdlRect.w = screen_w;  
			sdlRect.h = screen_h;  
			
			SDL_RenderClear( sdlRenderer );   
			SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);  
			SDL_RenderPresent( sdlRenderer );  
			//Delay 16ms
			SDL_Delay(16);
			
		}
        else if(event.type==SDL_WINDOWEVENT)
        {
			//If Resize
			SDL_GetWindowSize(screen,&screen_w,&screen_h);
		}else if(event.type==SDL_QUIT)
        {
			break;
		}
	}
 
	return 0;
}

void display_run(void)
{
    static uint32_t color = 0XFFFF0000, *bf_bf = NULL; // ARGB
    memset(buffer, 0XFF, sizeof(buffer));
    bf_bf = (uint32_t *)buffer;

    if (color <= 0XFFFF0F0F)
        color += 0X00100010;
    else
        color = 0XFF1F0F0F;

    for (size_t ii = 0; ii < sizeof(buffer) / 4; ii++)
        bf_bf[ii] = color_set;

    SDL_UpdateTexture(sdlTexture, NULL, buffer, pixel_w * 4);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
}

/**
 * It will be called from the main SDL thread
 */
void mouse_handler(SDL_Event * event)
{
    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
            {
                // left_button_down = false;
                printf("left_button_down = false\r\n");
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                // left_button_down = true;
                // last_x = event->motion.x / MONITOR_ZOOM;
                // last_y = event->motion.y / MONITOR_ZOOM;
                printf("left_button_down = true\r\n");
                printf("x: %d, y: %d\r\n", event->motion.x, event->motion.y);
                if (event->motion.x>=0&&event->motion.x<=pixel_w/2)
                {
                    color_set = 0XFFFF0000;
                }
                else if (event->motion.x>pixel_w/2&&event->motion.x<=pixel_w)
                {
                    color_set = 0XFF0000FF;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            // last_x = event->motion.x / MONITOR_ZOOM;
            // last_y = event->motion.y / MONITOR_ZOOM;
            printf("x: %d, y: %d\r\n", event->motion.x, event->motion.y);
            break;

        case SDL_FINGERUP:
            // left_button_down = false;
            // last_x = LV_HOR_RES * event->tfinger.x / MONITOR_ZOOM;
            // last_y = LV_VER_RES * event->tfinger.y / MONITOR_ZOOM;
            break;
        case SDL_FINGERDOWN:
            // left_button_down = true;
            // last_x = LV_HOR_RES * event->tfinger.x / MONITOR_ZOOM;
            // last_y = LV_VER_RES * event->tfinger.y / MONITOR_ZOOM;
            break;
        case SDL_FINGERMOTION:
            // last_x = LV_HOR_RES * event->tfinger.x / MONITOR_ZOOM;
            // last_y = LV_VER_RES * event->tfinger.y / MONITOR_ZOOM;
            break;
    }

}

int timer_count = 0;
 
//回调函数
uint32_t my_fun(uint32_t interval, void *param)
{
    printf("%u\n", SDL_GetTicks());
    printf("interval: %d  *param: %d\n", interval, *(int*)param);
    timer_count++;
    return interval;
}
 
int test_sdl_timer(void)
{
    int ret;
    uint32_t time;
    //定时器ID
    SDL_TimerID timeId = 0;
 
    //SDL初始化
    // ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    // if (-1 == ret)
    // {
    //     printf("SDL_Init failed..\n"); 
    //     goto err0;
    // }

    //延时函数  单位是毫秒
    //SDL_Delay(1000);
    //返回从SDL初始化开始到现在的时间 单位是毫秒
    time = SDL_GetTicks();
    printf("%u\n", time);
 
 
    //添加定时器
    timeId = SDL_AddTimer(1000, my_fun, &time); //定时1s
    if (0 == timeId)
    {
        printf("AddTime failed...\n"); 
        goto err0;
    }
    else
    {
        printf("AddTime ok...\n"); 
    }

    return 0;
    while(1)
    {
        if (10 == timer_count)
            break;
    }
 
    //删除定时器
    SDL_RemoveTimer(timeId);
 
    //关闭SDL
    SDL_Quit();
    return 0;
err0:
    return -1;
}

void do_loop(void *arg)
{
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    // lv_task_handler();

    SDL_Event event;
    
    while(SDL_PollEvent(&event)) {
        mouse_handler(&event);
    }

    display_run();
    sound_run();
    SDL_Delay(10); // Delay 10ms
}
