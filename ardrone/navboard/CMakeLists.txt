cmake_minimum_required(VERSION 2.6)

include ( ../ardrone_toolchain_setup.cmake )

link_libraries(m)

add_executable(navboard
	navboard.c
	main_navboard.c
	../util/util.c
	../gpio/gpio.c
)
