cmake_minimum_required(VERSION 2.6)

include ( ../ardrone_toolchain_setup.cmake )

add_executable(vbat
	vbat.c
	main_vbat.c
)
