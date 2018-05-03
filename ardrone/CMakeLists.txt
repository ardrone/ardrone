cmake_minimum_required(VERSION 2.6)

include ( ardrone_toolchain_setup.cmake )

SUBDIRS(
    fly
    navboard
    vbat
    attitude
    motorboard
    udp
)
