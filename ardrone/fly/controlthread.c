/*
    controlthread.c - AR.Drone control thread

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
#include <pthread.h>
#include <math.h>

#include "../util/type.h"
#include "../util/util.h"
#include "../motorboard/mot.h"
#include "../attitude/attitude.h"
#include "../udp/udp.h"
#include "pid.h"
#include "controlthread.h"

      float adj_roll;
      float adj_pitch;
      float adj_yaw;
      float adj_h;


pthread_t ctl_thread;

pid_struct pid_roll;
pid_struct pid_pitch;
pid_struct pid_yaw;
pid_struct pid_h;

float throttle;

att_struct att;

struct setpoint_struct {
  float pitch;     //radians  
  float roll;      //radians     
  float yaw;     //yaw in radians   
  float h;         //cm
  float pitch_roll_max; //radians     
  float h_max; //cm
  float h_min; //cm
  float throttle_hover; //hover throttle setting
  float throttle_min; //min throttle (while flying)
  float throttle_max; //max throttle (while flying)
} setpoint;

udp_struct udpNavLog;
int logcnt=0;
void navLog_Send();
void *ctl_thread_main(void* data);

int ctl_Init(char *client_addr) 
{
	int rc;
  
	//defaults from AR.Drone app:  pitch,roll max=12deg; yawspeed max=100deg/sec; height limit=on; vertical speed max=700mm/sec; 
	setpoint.pitch_roll_max=DEG2RAD(12); //degrees     
  //setpoint.yawsp_max=DEG2RAD(100); //degrees/sec
  setpoint.h_max=600; //cm
  setpoint.h_min=40; //cm
  setpoint.throttle_hover=0.66;
  setpoint.throttle_min=0.50;
  setpoint.throttle_max=0.85;
  			
	//init pid pitch/roll 
	pid_Init(&pid_roll,  0.50,0,0,0);
	pid_Init(&pid_pitch, 0.50,0,0,0);
	pid_Init(&pid_yaw,   1.00,0,0,0);
	pid_Init(&pid_h,     0.0005,0,0,0);

  throttle=0.00;

  //Attitude Estimate
	rc = att_Init(&att);
	if(rc) return rc;

  
  //udp logger
  udpClient_Init(&udpNavLog, client_addr, 7778);
  navLog_Send();
  printf("udpClient_Init\n", rc);
  
	//start motor thread
	rc = mot_Init();
	if(rc) return rc;
  
	//start ctl thread 
	rc = pthread_create(&ctl_thread, NULL, ctl_thread_main, NULL); 
	if(rc) {
		printf("ctl_Init: Return code from pthread_create(mot_thread) is %d\n", rc);
		return 202;
	}
}

void *ctl_thread_main(void* data)
{
	int cnt;
	int rc;

 	while(1) {
		rc = att_GetSample(&att);
		if(!rc) break;
		if(rc!=1) printf("ctl_thread_main: att_GetSample return code=%d",rc); 
	}	
    
	while(1) {
		//get sample
		while(1) {
			rc = att_GetSample(&att); //non blocking call
			if(!rc) break; //got a sample
			if(rc!=1) printf("ctl_thread_main: att_GetSample return code=%d",rc); 
		}
		
    float motor[4];    
    if(setpoint.h==0.00) {
      //motors off
      adj_roll = 0;
      adj_pitch = 0;
      adj_h = 0;
      adj_yaw = 0;
      throttle = 0;
    }else{     
      //flying, calc pid controller corrections
      adj_roll  = pid_CalcD(&pid_roll,  setpoint.roll   - att.roll,  att.dt, att.gx); //err positive = need to roll right
      adj_pitch = pid_CalcD(&pid_pitch, setpoint.pitch  - att.pitch, att.dt, att.gy); //err positive = need to pitch down
      adj_yaw   = pid_CalcD(&pid_yaw,   setpoint.yaw    - att.yaw,   att.dt, att.gz); //err positive = need to increase yaw to the left
      adj_h     = pid_CalcD(&pid_h,     setpoint.h      - att.h,     att.dt, att.hv); //err positive = need to increase height
      
      throttle = setpoint.throttle_hover + adj_h;
      if(throttle < setpoint.throttle_min) throttle = setpoint.throttle_min;
      if(throttle > setpoint.throttle_max) throttle = setpoint.throttle_max;      
    }
    
    //convert pid adjustments to motor values
    motor[0] = throttle +adj_roll -adj_pitch +adj_yaw;
    motor[1] = throttle -adj_roll -adj_pitch -adj_yaw;
    motor[2] = throttle -adj_roll +adj_pitch +adj_yaw;
    motor[3] = throttle +adj_roll +adj_pitch -adj_yaw;

    //send to motors
    mot_Run(motor[0],motor[1],motor[2],motor[3]);
        
    //blink leds    
    cnt++;
    if((cnt%200)==0) 
      mot_SetLeds(MOT_LEDGREEN,MOT_LEDGREEN,MOT_LEDGREEN,MOT_LEDGREEN);
    else if((cnt%200)==100) 
      mot_SetLeds(0,0,0,0);
        
    //send UDP nav log packet    
    navLog_Send();
  
		//yield to other threads
		pthread_yield();
	}
}


//logging
void navLog_Send()
{
  char logbuf[1024];
  int logbuflen;

  float motval[4];
	mot_GetMot(motval);
    
  logcnt++;
  logbuflen=sprintf(logbuf,"%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
    //sequence+timestamp
    ,logcnt
    ,att.ts   // navdata timestamp in sec
    //sensor data
    ,att.ax   // acceleration x-axis in [G] front facing up is positive         
    ,att.ay   // acceleration y-axis in [G] left facing up is positive                
    ,att.az   // acceleration z-axis in [G] top facing up is positive             
    ,RAD2DEG(att.gx)   // gyro value x-axis in [deg/sec] right turn, i.e. roll right is positive           
    ,RAD2DEG(att.gy)   // gyro value y-axis in [deg/sec] right turn, i.e. pirch down is positive                     
    ,RAD2DEG(att.gz)   // gyro value z-axis in [deg/sec] right turn, i.e. yaw left is positive 
    ,att.hv   // vertical speed [cm/sec]
    //height
    ,setpoint.h  // setpoint height
    ,att.h       // actual height above ground in [cm] 
    ,throttle    // throttle setting 0.00 - 1.00
    //pitch
    ,RAD2DEG(setpoint.pitch)  //setpoint pitch [deg]
    ,RAD2DEG(att.pitch)       //actual pitch   
    ,adj_pitch                //pitch motor adjustment 
    //roll
    ,RAD2DEG(setpoint.roll)   //setpoint roll [deg]
    ,RAD2DEG(att.roll)        //actual roll  
    ,adj_roll                 //roll motor adjustment 
    //yaw
    ,RAD2DEG(setpoint.yaw)    //yaw pitch [deg]
    ,RAD2DEG(att.yaw)         //actual yaw  
    ,adj_yaw                  //yaw motor adjustment
  );    
  udpClient_Send(&udpNavLog,logbuf,logbuflen); 
}

int ctl_FlatTrim()
{
  return att_FlatTrim(&att);
}

void ctl_SetSetpoint(float pitch, float roll, float yaw, float h)
{
  if(pitch > setpoint.pitch_roll_max) pitch = setpoint.pitch_roll_max;
  if(pitch < -setpoint.pitch_roll_max) pitch = -setpoint.pitch_roll_max;
  setpoint.pitch=pitch;
  if(roll > setpoint.pitch_roll_max) roll = setpoint.pitch_roll_max;
  if(roll < -setpoint.pitch_roll_max) roll = -setpoint.pitch_roll_max;
  setpoint.roll=roll;
  //if(yaw > setpoint.yawsp_max) yaw = setpoint.yawsp_max;
  //if(yaw < -setpoint.yawsp_max) yaw = -setpoint.yawsp_max;
  setpoint.yaw=yaw;
  if(h > setpoint.h_max) h = setpoint.h_max;
  if(h <= 0) h = 0;
  if(h>0 && h < setpoint.h_min) h = setpoint.h_min;
  if(setpoint.h==0 && h>0) throttle=0.69; //takeoff
  setpoint.h=h;
}

void ctl_SetSetpointDiff(float pitch, float roll, float yaw, float h)
{
  ctl_SetSetpoint(pitch+setpoint.pitch, setpoint.pitch+pitch, yaw+setpoint.yaw, h+setpoint.h);
}

void ctl_Close()
{
  mot_Close();
  att_Close();
}

void ctl_SetGas(float gas1)
{
	throttle+=gas1;
}

