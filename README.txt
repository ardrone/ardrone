Introduction
------------
This project provides an open source firmware for the AR.Drone by Parrot. 

WARNING: This project is not supported nor endorsed by Parrot S.A., using the this sofware will most likely void your warranty and might destroy your drone or make it fly away into deep space. Use at your own risk. 

The new firmware for the drone is called 'fly'. Upon start it will listen on UDP port 7777, waiting for the pc based contoller called 'OpenFlight' to connect. Once OpenFlight connects, 'fly' will kill the Parrot firmware 'program.elf' and takes over control of the drone. OpenFlight logs the flight data to a file called YYYYMMDDHHMMSS_navlog.csv.

Please post feedback on http://blog.perquin.com/blog/category/ardrone/ Thanks!


Known Issues
------------
The PID controller parameters have not been optimized yet and currently use 'P' parameters only. In particular the height controller will probably undershoot it's target height, or the drone might not have enough power to take off, this because of the missing 'I' term. 


Install
-------
1) FTP the folder ardrone/bin to your drone

2) Open a telnet session to your drone and execute: 
# chmod 777 /data/video/bin/*
# /data/video/bin/fly

3) On your pc, start OpenFlight.exe and follow the instructions on screen.

4) (Optional) Execute the following command to make 'fly' start everytime you powerup the drone:
# echo -e "\n\n/data/video/bin/fly &" >> /etc/init.d/rcS


Other telnet commandline demo programs on the drone are:
You'll need to execute `killall program.elf` before using these programs!
# /data/video/bin/navboard          - to see the navboard raw sensor data (3-axis accelleration, 3-axis gyro, sonar echos) 
# /data/video/bin/attitude          - to see the navboard attitude data (pitch angle, roll angle, yaw angle, height)
# /data/video/bin/motorboard        - to control the motors/leds 
# /data/video/bin/vbat              - to see the on board voltages 
# /data/video/bin/video             - turns the drone into a giant optical mouse, keep it about 1 meter above ground and move it parallel to the ground. The program should report the movement.


Directories
-----------
ardrone/bin        - Pre-compiled binaries. FTP these files to your drone and run them in a telnet session.
ardrone/fly        - Custom firmware to fly the drone, the controller is under pc/OpenFlight
ardrone/navboard   - Navboard driver and demo getting raw data from the navboard and converting it to realworld units (works)
ardrone/attitude   - Attitude driver and demo calculating attitude (pitch,roll,yaw and height) estimates from navboard data (works)
ardrone/motorboard - Motorboard driver and demo control of the 4 motors and leds (works)
ardrone/vbat       - Battery voltage driver and demo (works)
ardrone/gpio       - GPIO driver and demo (works)
ardrone/video      - Video driver and demo (works, but needs start&kill of program.elf)
ardrone/util       - Utilities
pc/OpenFlight      - C# controller for /ardrone/fly


Compiling
---------
For ardrone directory:
1) Download and install arm-2011.03-41-arm-none-linux-gnueabi.exe "Sourcery G++ Lite 2011.03-41 for ARM GNU/Linux" from http://www.codesourcery.com/sgpp/lite/arm/portal/release1803
2) On Windows: open a command prompt and cd to one of the directories
3) Run the make.bat batch file to compile the program in that directory

For pc directory
1) Download and install Visual Studio C# 2010 Express
2) Open pc/OpenFlight.sln


Changelog
---------
19-Sep-2011 Added ardrone/fly and pc/OpenFlight
20-Jul-2011 Added video
17-Jul-2011 Initial release
