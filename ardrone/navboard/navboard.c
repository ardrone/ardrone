/*
    navboard.c - AR.Drone navboard driver

    Copyright (C) 2011 Hugo Perquin - http://blog.perquin.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>  //exit()
#include <math.h>

#include "navboard.h"
#include "../gpio/gpio.h"
#include "../util/util.h"

int nav_fd;

float accs_offset[]                    = { 2048, 2048, 2048 };
const float accs_gains[]               = { 1024, 1024, 1024 };
float gyros_offset[]                   = { 1350/0.806, 1350/0.806, 1350/0.806 };
const float gyros_gains[]              = { 6.9786031e-03, 7.1979444e-03, 3.8175473e-03 }; 
float gyros110_offset[]                = { 1350/0.806, 1350/0.806 };
const float gyros110_gains[]           = { 1.5517747e-03, 1.5981209e-03 };

//const float accs_offset[]                    = { -2.2074473e+03, 1.9672749e+03, 1.9423679e+03 };
//const float accs_gains[]                     = {  9.6180552e-01,  2.5897421e-02,  5.7041653e-02,  
//                                                 -1.2127322e-02, -9.9142015e-01,  2.2923036e-02,  
//												    4.8988122e-02, -4.6047132e-02, -9.6375960e-01 };
//const float gyros_offset[]                   = { 1.6654420e+03, 1.6706140e+03, 1.6645740e+03 };
//const float gyros_gains[]                    = { 6.9786031e-03, -7.1979444e-03, -3.8175473e-03 };
//const float gyros110_offset[]                = { 1.6567020e+03, 1.6819180e+03 };
//const float gyros110_gains[]                 = { 1.5517747e-03, -1.5981209e-03 };

//get a sample from the nav board (non blocking)
//returns 0 on success
int nav_GetSample(nav_struct* nav)
{
	int n;
	do {
		//http://www.easysw.com/~mike/serial/serial.html
		//When you operate the port in raw data mode, each read(2) system call will return the number 
		//of characters that are actually available in the serial input buffers. If no characters are 
		//available the call will return immediately due to fcntl(fd, F_SETFL, FNDELAY)
		//The FNDELAY option causes the read function to return 0 if no characters are available on the port.
		n = read(nav_fd, nav, 46);
		if(n<46) return 1; //no packet received
	} while(n - 46 > 46);  //loop until last full packet is read
	//check data is valid
	u16 checksum	
		=nav->seq
		+nav->acc[0]
		+nav->acc[1]
		+nav->acc[2]
		+nav->gyro[0]
		+nav->gyro[1]
		+nav->gyro[2]
		+nav->gyro_110[0]
		+nav->gyro_110[1]
		+nav->acc_temp
		+nav->gyro_temp
		+nav->vrefEpson
		+nav->vrefIDG
		+nav->us_echo
		+nav->us_echo_start
		+nav->us_echo_end
		+nav->us_association_echo
		+nav->us_distance_echo
		+nav->us_courbe_temps
		+nav->us_courbe_valeur
		+nav->us_courbe_ref; 
	
	if(nav->size!=44) return 2; //size incorrect
	if(nav->checksum!=checksum) return 3; //checksum incorrect
	
	//store timestamp
	double ts_prev = nav->ts;
	nav->ts = util_timestamp();
	nav->dt = nav->ts - ts_prev;
	
	//store converted sensor data in nav structure. 
	nav->ax = (((float)nav->acc[0]) - accs_offset[0]) / accs_gains[0];
	nav->ay = (((float)nav->acc[1]) - accs_offset[1]) / accs_gains[1];
	nav->az = (((float)nav->acc[2]) - accs_offset[2]) / accs_gains[2];
	nav->gx = (((float)nav->gyro[0]) - gyros_offset[0]) * gyros_gains[0];
	nav->gy = (((float)nav->gyro[1]) - gyros_offset[1]) * gyros_gains[1];
	nav->gz = (((float)nav->gyro[2]) - gyros_offset[2]) * gyros_gains[2];	
	if(nav->gx>DEG2RAD(-100) && nav->gx<DEG2RAD(100)) nav->gx = ((float)nav->gyro_110[0] - gyros110_offset[0]) * gyros110_gains[0];
	if(nav->gy>DEG2RAD(-100) && nav->gy<DEG2RAD(100)) nav->gy = ((float)nav->gyro_110[1] - gyros110_offset[1]) * gyros110_gains[1];
	nav->h  = (float)((nav->us_echo&0x7fff)) * 0.0340;
  nav->h_meas = nav->us_echo >> 15;
	nav->tg  = (( (float)nav->gyro_temp * 0.806 /*mv/lsb*/ ) - 1250 /*Offset 1250mV at room temperature*/) / 4.0 /*Sensitivity 4mV/°C*/ + 20 /*room temperature*/;
	nav->ta  = ((float)nav->acc_temp) * 0.5 /*C/lsb*/ - 30 /*Offset is -30C*/;
	
	return 0;
}

void nav_Print(nav_struct* nav) 
{
/*
	printf("RAW seq=%d a=%d,%d,%d g=%d,%d,%d,%d,%d h=%d ta=%d tg=%d\n"
		,nav->seq
		,nav->acc[0],nav->acc[1],nav->acc[2]
		,nav->gyro[0],nav->gyro[1],nav->gyro[2],nav->gyro_110[0],nav->gyro_110[1]
		,nav->us_echo
		,nav->acc_temp
		,nav->gyro_temp
	);
*/	
	printf("a=%6.3f,%6.3f,%6.3fG g=%4.0f,%4.0f,%4.0fdeg/s h=%3.0fcm ta=%4.1fC tg=%4.1fC dt=%2.0fms\n"
		,nav->seq
		,nav->ax,nav->ay,nav->az
		,RAD2DEG(nav->gx),RAD2DEG(nav->gy),RAD2DEG(nav->gz)
		,nav->h
		,nav->ta
		,nav->tg
		,nav->dt*1000
	);
}


//calibrate offsets. Drone has to be horizontal and stationary
//TODO add timeout
//returns 0 on success
int nav_FlatTrim() 
{
	accs_offset[0]=2122.639893;
	accs_offset[1]=1978.560059;
	accs_offset[2]=2012.040039;
	gyros_offset[0]=1670.439941;
	gyros_offset[1]=1664.010010;
	gyros_offset[2]=1658.079956;
	gyros110_offset[0]=1690.079956;
	gyros110_offset[1]=1662.890015;
	//printf("nav_Calibrate bypassed\n");
	//return 0;

	nav_struct nav;
	int n_samples=40;
	int n=0; //number of samples
	float x1[8],x2[8]; //sum and sqr sum
	float avg[8],std[8]; //average and standard deviation
	int i;
		
	//zero sums
	for(i=0;i<8;i++) {x1[i]=0;x2[i]=0;}
	
	//collect n_samples samples 
	while(n<n_samples) {
		int retries=1;
		while(retries<100) {
			int rc = nav_GetSample(&nav);
			if(rc==0) break;
			retries++;
			printf("nav_Calibrate: retry=%d, code=%d\r\n",retries,rc); 
		}
		n++,
		x1[0]+=(float)nav.acc[0];
		x2[0]+=(float)nav.acc[0]*(float)nav.acc[0];
		x1[1]+=(float)nav.acc[1];
		x2[1]+=(float)nav.acc[1]*(float)nav.acc[1];
		x1[2]+=(float)nav.acc[2];
		x2[2]+=(float)nav.acc[2]*(float)nav.acc[2];
		x1[3]+=(float)nav.gyro[0];
		x2[3]+=(float)nav.gyro[0]*(float)nav.gyro[0];
		x1[4]+=(float)nav.gyro[1];
		x2[4]+=(float)nav.gyro[1]*(float)nav.gyro[1];
		x1[5]+=(float)nav.gyro[2];
		x2[5]+=(float)nav.gyro[2]*(float)nav.gyro[2];
		x1[6]+=(float)nav.gyro_110[0];
		x2[6]+=(float)nav.gyro_110[0]*(float)nav.gyro_110[0];
		x1[7]+=(float)nav.gyro_110[1];
		x2[7]+=(float)nav.gyro_110[1]*(float)nav.gyro_110[1];
	}
	
	//calc avg and standard deviation
	for(i=0;i<8;i++) {
	  avg[i]=x1[i]/n;
	  std[i]=(x2[i]-x1[i]*x1[i]/n)/(n-1);
	  if(std[i]<=0) std[i]=0; else std[i]=sqrt(std[i]); //handle rounding errors
	}
	
	//validate
	for(i=0;i<3;i++) if(std[i]>10) return 1+i; //validate accs
	for(i=3;i<8;i++) if(std[i]>10) return 1+i; //validate gyros
	int tol=120;
	if(avg[0]<2048-tol || avg[0]>2048+tol) {printf("nav_Calibrate: ax_avg out of tolerance: %d\r\n",avg[0]); return 10;}
	if(avg[1]<2048-tol || avg[1]>2048+tol) {printf("nav_Calibrate: ay_avg out of tolerance: %d\r\n",avg[1]); return 11;}
	if(avg[2]<3096-tol || avg[2]>3096+tol) {printf("nav_Calibrate: az_avg out of tolerance: %d\r\n",avg[2]); return 12;}
	
	//set offsets
	accs_offset[0]=avg[0];
	accs_offset[1]=avg[1];
	accs_offset[2]=avg[2]-accs_gains[2]; 
	gyros_offset[0]=avg[3];
	gyros_offset[1]=avg[4];
	gyros_offset[2]=avg[5];
	gyros110_offset[0]=avg[6];
	gyros110_offset[1]=avg[7];
	
	printf("nav_Calibrate: accs_offset=%f,%f,%f\n",accs_offset[0],accs_offset[1],accs_offset[2]);
	printf("nav_Calibrate: gyros_offset=%f,%f,%f\n",gyros_offset[0],gyros_offset[1],gyros_offset[2]);
	printf("nav_Calibrate: gyros110_offset=%f,%f\n",gyros110_offset[0],gyros110_offset[1]);
	
	return 0;	
}

int nav_Init(nav_struct* nav) {
	//open nav port
	//stty -F /dev/ttyPA2 460800 -parenb -parodd cs8 -hupcl -cstopb cread clocal -crtscts 
	//-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl -ixon -ixoff -iuclc -ixany -imaxbel 
	//-opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0 -isig -icanon -iexten 
	//-echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke

	nav_fd = open("/dev/ttyPA2", O_RDWR | O_NOCTTY | O_NDELAY);
	if (nav_fd == -1)
	{
		perror("nav_Init: Unable to open /dev/ttyPA2 - ");
		return 101;
	} 
	fcntl(nav_fd, F_SETFL, 0); //read calls are non blocking
	//set port options
	struct termios options;
	//Get the current options for the port
	tcgetattr(nav_fd, &options);
	//Set the baud rates to 460800
	cfsetispeed(&options, B460800);
	cfsetospeed(&options, B460800);
/*Control Options  (options.c_cflag)
B0 0 baud (drop DTR) 
B50 50 baud 
B75 75 baud 
B110 110 baud 
B134 134.5 baud 
B150 150 baud 
B200 200 baud 
B300 300 baud 
B600 600 baud 
B1200 1200 baud 
B1800 1800 baud 
B2400 2400 baud 
B4800 4800 baud 
B9600 9600 baud 
B19200 19200 baud 
B38400 38400 baud 
B57600 57,600 baud 
B76800 76,800 baud 
B115200 115,200 baud 
EXTA External rate clock 
EXTB External rate clock 
CSIZE Bit mask for data bits 
CS5 5 data bits 
CS6 6 data bits 
CS7 7 data bits 
CS8 8 data bits 
CSTOPB 2 stop bits (1 otherwise) 
CREAD Enable receiver 
PARENB Enable parity bit 
PARODD Use odd parity instead of even 
HUPCL Hangup (drop DTR) on last close 
CLOCAL Local line - do not change "owner" of port 
LOBLK Block job control output 
CNEW_RTSCTS 
CRTSCTS Enable hardware flow control (not supported on all platforms) 
*/
	options.c_cflag |= (CLOCAL | CREAD); //Enable the receiver and set local mode
/*Input Options (options.c_iflag)
INPCK Enable parity check 
IGNPAR Ignore parity errors 
PARMRK Mark parity errors 
ISTRIP Strip parity bits 
IXON Enable software flow control (outgoing) 
IXOFF Enable software flow control (incoming) 
IXANY Allow any character to start flow again 
IGNBRK Ignore break condition 
BRKINT Send a SIGINT when a break condition is detected 
INLCR Map NL to CR 
IGNCR Ignore CR 
ICRNL Map CR to NL 
IUCLC Map uppercase to lowercase 
IMAXBEL Echo BEL on input line too long 
*/
	options.c_iflag = 0; //clear input options
/*Local Options (options.c_lflag)
ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals 
ICANON Enable canonical input (else raw) 
XCASE Map uppercase \lowercase (obsolete) 
ECHO Enable echoing of input characters 
ECHOE Echo erase character as BS-SP-BS 
ECHOK Echo NL after kill character 
ECHONL Echo NL 
NOFLSH Disable flushing of input buffers after interrupt or quit characters 
IEXTEN Enable extended functions 
ECHOCTL Echo control characters as ^char and delete as ~? 
ECHOPRT Echo erased character as character erased 
ECHOKE BS-SP-BS entire line on line kill 
FLUSHO Output being flushed 
PENDIN Retype pending input at next read or input char 
TOSTOP Send SIGTTOU for background output 
*/
	options.c_lflag=0; //clear local options
/*Output Options (options.c_oflag)
OPOST Postprocess output (not set = raw output) 
OLCUC Map lowercase to uppercase 
ONLCR Map NL to CR-NL 
OCRNL Map CR to NL 
NOCR No CR output at column 0 
ONLRET NL performs CR function 
OFILL Use fill characters for delay 
OFDEL Fill character is DEL 
NLDLY Mask for delay time needed between lines 
NL0 No delay for NLs 
NL1 Delay further output after newline for 100 milliseconds 
CRDLY Mask for delay time needed to return carriage to left column 
CR0 No delay for CRs 
CR1 Delay after CRs depending on current column position 
CR2 Delay 100 milliseconds after sending CRs 
CR3 Delay 150 milliseconds after sending CRs 
TABDLY Mask for delay time needed after TABs 
TAB0 No delay for TABs 
TAB1 Delay after TABs depending on current column position 
TAB2 Delay 100 milliseconds after sending TABs 
TAB3 Expand TAB characters to spaces 
BSDLY Mask for delay time needed after BSs 
BS0 No delay for BSs 
BS1 Delay 50 milliseconds after sending BSs 
VTDLY Mask for delay time needed after VTs 
VT0 No delay for VTs 
VT1 Delay 2 seconds after sending VTs 
FFDLY Mask for delay time needed after FFs 
FF0 No delay for FFs 
FF1 Delay 2 seconds after sending FFs 
*/
	options.c_oflag &= ~OPOST; //clear output options (raw output)
	//Set the new options for the port
	tcsetattr(nav_fd, TCSANOW, &options);
  
	//set /MCLR pin
	gpio_set(132,1);
	
	//start acquisition
	u08 cmd=0x01;
	write(nav_fd,&cmd,1);
  
	//init nav structure	
    nav->ts = util_timestamp();
    nav->dt = 0;
	
	return 0;
}

void nav_Close()
{
	close(nav_fd);
}