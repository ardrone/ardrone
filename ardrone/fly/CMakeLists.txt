cmake_minimum_required(VERSION 2.6)

include (../ardrone_toolchain_setup.cmake )

link_libraries(pthread)
link_libraries(m)


SET( FLY_SOURCES 	main_fly.c
	controlthread.c
    pid.c
	../navboard/navboard.c
	../util/util.c
	../gpio/gpio.c
	../motorboard/mot.c
	../motorboard/motorboard.c
	../udp/udp.c
	../attitude/attitude.c
	../attitude/ars.c
	../video/video.c
)

add_executable(fly
	${FLY_SOURCES}
)

add_executable(nofly
	${FLY_SOURCES}
)
SET_TARGET_PROPERTIES (nofly PROPERTIES COMPILE_FLAGS "-DMOTOR_DRY_RUN")
