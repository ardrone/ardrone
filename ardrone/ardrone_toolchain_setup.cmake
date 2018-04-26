SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   arm-none-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mthumb -mfloat-abi=softfp -fomit-frame-pointer -pipe -ffunction-sections -fdata-sections")
