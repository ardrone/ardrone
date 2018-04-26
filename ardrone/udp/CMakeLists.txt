cmake_minimum_required(VERSION 2.6)

include ( ../ardrone_toolchain_setup.cmake )

link_libraries(m)

add_executable(udpclient
	main_udpclient.c
	udp.c
)

add_executable(udpserver
	main_udpserver.c
	udp.c
)
