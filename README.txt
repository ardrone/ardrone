Introduction
------------
Goal of this project is to provide a custom firmware for the AR.Drone. Currently the drivers for the motorboard and navboard are working.
Please post feedback/improvements on http://blog.perquin.com/blog/category/ardrone/ Thanks!

Quick Setup
-----------
ftp the folder bin to your drone
telnet to your drone and execute: 
# killall program.elf
# /data/video/bin/navboard          - to see the navboard raw data
# /data/video/bin/attitude          - to see the navboard physical data
# /data/video/bin/motorboard        - to control the motors/leds 
# /data/video/bin/vbat              - to see the voltages 
# /data/video/bin/video             - turns the drone into a giant optical mouse, keep it about 1 meter above ground and move it parallel to the ground. The program should report the movement.

Directories
-----------
bin        - pre-compiled binaries. FTP these files to your drone and run them in a telnet session.
navboard   - navboard driver and demo getting raw data from the navboard and converting it to realworld units (works)
attitude   - attitude driver and demo calculating attitude (pitch,roll,yaw and height) estimates from navboard data (works)
motorboard - motorboard driver and demo control of the 4 motors and leds (works)
vbat       - battery voltage driver and demo (works)
gpio       - gpio driver and demo (works)
video      - video driver and demo (works, but needs start&kill of program.elf)
util       - utilities

Compiling
---------
1) Download and install arm-2011.03-41-arm-none-linux-gnueabi.exe "Sourcery G++ Lite 2011.03-41 for ARM GNU/Linux" from http://www.codesourcery.com/sgpp/lite/arm/portal/release1803
2) On Windows: open a command prompt and cd to one of the directories
3) Run the make.bat batch file to compile the program in that directory

Changelog
---------
20-Jul-2011 Added video
17-Jul-2011 Initial release
