cmake_minimum_required(VERSION 2.6)

include ( ../ardrone_toolchain_setup.cmake )

link_libraries(m)
link_libraries(pthread)

add_executable(motorboard
	main_motorboard.c
	motorboard.c
	mot.c
	../util/util.c
	../gpio/gpio.c
)
