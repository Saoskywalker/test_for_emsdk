#ifndef _TEST_SDL_H
#define _TEST_SDL_H

#include <stdio.h>
#include <stdint.h>

void test_sdl(void);
int test_sdl_audio(int argc, char* argv[]);
int test_sdl_framebuffer(int argc, char* argv[]);
void do_loop(void *arg);

#endif
