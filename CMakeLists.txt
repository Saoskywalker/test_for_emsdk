#限制最低cmake版本
cmake_minimum_required(VERSION 3.12)

#cmake工程名字 
project (emsdk_test)

#设置C编译器特性
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s USE_SDL=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s USE_SDL_NET=2 -s FORCE_FILESYSTEM=1")

#宏定义, 类似gcc的-D
add_definitions(-DDEBUG)

#设置头文件路径, 相当于gcc的-I
include_directories(include)

#设置库文件路径, 相当于gcc的-L
# link_directories(lib)

#添加子cmakelists文件
# add_subdirectory(lvgl)
# add_subdirectory(lv_drivers)

#扫描指定文件并添加到编译文件目录
# file(GLOB MY_SOURCES "./*.c")
# set(SOURCES ${MY_SOURCES})

#设置带编译文件, 可包括C/C++
set(SRC_LIST main.c)
set(SRC_LIST ${SRC_LIST} test_sdl.c)

#编译生成名为index可执行文件
add_executable(index ${SRC_LIST})

#按输入参数选择
# if(NOT LVGL_CHOSEN_DEMO)
#     set(LVGL_CHOSEN_DEMO lv_demo_widgets)
# endif()

#设置某个文件特性
# set_source_files_properties(main.c PROPERTIES COMPILE_FLAGS -DCHOSEN_DEMO=${LVGL_CHOSEN_DEMO})

#设置可执行文件后缀
set(CMAKE_EXECUTABLE_SUFFIX ".html")

#index文件需要使用到的库名称, PS:使用GCC规则, 缩写名, 如libSDL2.so, 则为SDL2
# target_link_libraries(index
#     SDL2
# )

#设置运行文件特性
#按要求生成合适的HTML文件
set_target_properties(index PROPERTIES LINK_FLAGS "--shell-file ${PROJECT_SOURCE_DIR}/emsdk_shell.html -s SINGLE_FILE=0")
