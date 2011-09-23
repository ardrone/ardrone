/*
    main_motorboard.c - AR.Drone motor demo program

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
//#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
//#include <fcntl.h>   /* File control definitions */
//#include <errno.h>   /* Error number definitions */
//#include <termios.h> /* POSIX terminal control definitions */
//#include <stdlib.h>  //exit()
#include <pthread.h>
#include <ctype.h>    /* For tolower() function */
#include <math.h>

#include "../util/type.h"
#include "../util/util.h"
#include "mot.h"


int main()
{
  printf("Motorboard Test Program ... Press q to quit\r\n");
  printf("Motor: 1,2,3,4=run single motor at 50%  5=run all motors at 50% ,=throttle up 1%  .=throttle down 1%  space=stop\r\n");
  printf("Leds:  a=loff  s=green  d=orange  f=red\r\n");
	
  mot_Init();
  
  float throttle1 = 0;
  float throttle2 = 0;
  float throttle3 = 0;
  float throttle4 = 0;
  float step=0.01;
 
  //main loop	
  while(1) { 
    //handle user input
	int c=tolower(util_getch());
	if(c=='q') break;
	if(c=='1') {
		printf("\rRun Motor1 50%            ");
		throttle1 = .50;
		throttle2 = 0;
		throttle3 = 0;
		throttle4 = 0;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c=='2') {
		printf("\rRun Motor2 50%            ");
		throttle1 = 0;
		throttle2 = .50;
		throttle3 = 0;
		throttle4 = 0;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c=='3') {
		printf("\rRun Motor3 50%            ");
		throttle1 = 0;
		throttle2 = 0;
		throttle3 = .50;
		throttle4 = 0;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c=='4') {
		printf("\rRun Motor4 50%            ");
		throttle1 = 0;
		throttle2 = 0;
		throttle3 = 0;
		throttle4 = .50;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c=='5') {
		printf("\rRun All Motors 50%            ");
		throttle1 = .50;
		throttle2 = .50;
		throttle3 = .50;
		throttle4 = .50;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c==',') {
		printf("\rThrottle down            ");
		if(throttle1>step) throttle1 -= step;
		if(throttle2>step) throttle2 -= step;
		if(throttle3>step) throttle3 -= step;
		if(throttle4>step) throttle4 -= step;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c=='.') {
		printf("\rThrottle up            ");
		if(throttle1>0) throttle1 += step;
		if(throttle2>0) throttle2 += step;
		if(throttle3>0) throttle3 += step;
		if(throttle4>0) throttle4 += step;
		mot_Run(throttle1,throttle2,throttle3,throttle4);
	}
	if(c==' ') {
		printf("\rStop            ");
		mot_Stop();
	}
	if(c=='a') {
		printf("\rLeds off            ");
		mot_SetLeds(MOT_LEDOFF,MOT_LEDOFF,MOT_LEDOFF,MOT_LEDOFF);
	}
	if(c=='s') {
		printf("\rLeds green            ");
		mot_SetLeds(MOT_LEDGREEN,MOT_LEDGREEN,MOT_LEDGREEN,MOT_LEDGREEN);
	}
	if(c=='d') {
		printf("\rLeds orange            ");
		mot_SetLeds(MOT_LEDORANGE,MOT_LEDORANGE,MOT_LEDORANGE,MOT_LEDORANGE);
	}
	if(c=='f') {
		printf("\rLeds red            ");
		mot_SetLeds(MOT_LEDRED,MOT_LEDRED,MOT_LEDRED,MOT_LEDRED);
	}
	
	//yield to other threads
	pthread_yield();
  }
  mot_Close();
  printf("\nDone...\n");
  return 0;
}
