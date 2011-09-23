/*
    attitude.c - AR.Drone attitude estimate driver

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

#include "../util/type.h"
#include "../navboard/navboard.h"
#include "../util/util.h"
#include "ars.h"
#include "attitude.h"


float lr_a;
float lr_b;
int lr_n;
int lr_off;
float lr_y[3];

//init linear regession for n values
void lr_init(int n)
{
  lr_n=n;
  float sx=0;
  float sxx=0;
  for(int i=0;i<n;i++) {
    sx+=i;
    sxx+=i*i;
    lr_y[i]=0;
  }
  lr_a=sx/n;
  lr_b=sxx-sx*sx/n;
}

//calc slope for next y value
float lr_slope(float y) {
  float sxy=0;
  float sy=0;
  lr_y[lr_off]=y;
  lr_off = (lr_off+1)%lr_n;
  for(int i=0;i<lr_n;i++) {
    int ii=(i+lr_off)%lr_n;
    sy+=lr_y[ii];
    sxy+=lr_y[ii]*i;
  }
  return (sxy - lr_a*sy)/lr_b;
}





nav_struct nav;
ars_Gyro1DKalman ars_roll; 
ars_Gyro1DKalman ars_pitch; 

//roll angle from acc in radians
float roll(float a_z, float a_y)
{
	return atan2(a_y, a_z);
}

//pitch angle from acc in radians
float pitch(float a_z, float a_x)
{
	return -atan2(a_x, a_z);
}

void att_Print(att_struct *att)
{
	printf("roll=%5.1f,a=%5.1f,g=%5.1f pitch=%5.1f,a=%5.1f,g=%5.1f yaw=%5.1f h=%5.1f dt=%4.1f\n"
		,RAD2DEG(att->roll),  RAD2DEG(att->roll_a),  RAD2DEG(att->roll_g)
		,RAD2DEG(att->pitch), RAD2DEG(att->pitch_a), RAD2DEG(att->pitch_g)
		,RAD2DEG(att->yaw)
		,att->h
		,att->dt*1000
	);
}

float last_h;
float last_ts;

int att_GetSample(att_struct *att) 
{
	int rc;
	
	//get nav sample
	rc = nav_GetSample(&nav); 
	if(rc) return rc;
/*
	//get nav sample
	double starttime=-1;
	while(1) {
		rc = nav_GetSample(&nav); 
		if(!rc) break;
	    if(starttime<0) starttime=util_timestamp();
		if(util_timestamp()-starttime>=0.100) return rc;
	}
*/	
	//DEBUG introduce bias
	//nav.gx += att->dt * 10;
	//nav.gy -= att->dt * 20;

	//update dt and copy nav data
	att->dt = nav.ts - att->ts;
	att->ts = nav.ts;
	att->ax = nav.ax;
	att->ay = nav.ay;
	att->az = nav.az;
	att->gx = nav.gx;
	att->gy = nav.gy;
	att->gz = nav.gz;
  att->hraw = nav.h;
 	att->h_meas  = nav.h_meas;
 
  //smooth out missing h samples
  if(abs(nav.h-last_h)>5 && last_ts-nav.ts<0.10) {
    att->h = last_h;
  }else{  
	  att->h  = nav.h;
    last_h = nav.h;
    last_ts = nav.ts;
  }
  if(nav.h_meas) {
    att->hv= lr_slope(att->h)*25; //25Hz sample rate
  }
	
	//update att
	att->roll_g  += nav.gx * att->dt;
	att->pitch_g += nav.gy * att->dt;
	att->yaw     += nav.gz * att->dt;
	att->roll_a  = roll(nav.az,nav.ay);
	att->pitch_a = pitch(nav.az,nav.ax);
	
	//execute kalman roll filter
	ars_predict(&ars_roll, nav.gx, att->dt);               
	att->roll = ars_update(&ars_roll, att->roll_a);   
	
	//execute kalman pitch filter
	ars_predict(&ars_pitch, nav.gy, att->dt);               
	att->pitch = ars_update(&ars_pitch, att->pitch_a);   	

/*
	//DEBUG print bias
	printf("roll=%5.1f roll_a=%5.1f roll_g=%5.1f bias=%5.1f gx=%5.1f dt=%5.1f\n"
		,att->roll/3.1415926*180, att->roll_a/3.1415926*180, att->roll_g/3.1415926*180
		,ars_roll.x_bias/3.1415926*180
		,nav.gx/3.1415926*180
		,att->dt*1000
	);
*/	
	return 0;
}

int att_FlatTrim(att_struct *att) 
{
	int rc;
  //calibrate
	printf("Calibrating Navboard ...\n");
	rc=nav_FlatTrim();
	if(rc) printf("Failed: retcode=%d\r\n",rc); else printf("Calibrate Navboard OK\n");

	//init ars
  ars_Init(&ars_roll, 0.001/*Q_angle*/, 0.003/*Q_gyro*/, 0.69/*R_angle*/);
  ars_Init(&ars_pitch, 0.001/*Q_angle*/, 0.003/*Q_gyro*/, 0.69/*R_angle*/);
	
	//clear att
	att->pitch_g=0;
	att->pitch_a=0;
	att->pitch=0;
	att->roll_g=0;
	att->roll_a=0;
	att->roll=0;
	att->yaw=0;
	att->h=0;
	att->ts=util_timestamp();
	att->dt=0;
  
  return rc;
}

int att_Init(att_struct *att) 
{
	int rc;
	
  lr_init(3);
  
	//nav board
	printf("Init Navboard ...\n");
	rc = nav_Init(&nav);
	if(rc) return rc;
  
  rc = nav_GetSample(&nav); 
	if(rc) return rc;
  last_ts=nav.ts;
  last_h=nav.h;
  
	printf("Init Navboard OK\n");
		
	att_FlatTrim(att);
  return 0;
}

void att_Close()
{
	nav_Close();
}

