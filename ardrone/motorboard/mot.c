/*
    mot.c - AR.Drone motor thread

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
#include <errno.h>   /* Error number definitions */
#include <stdlib.h>  //exit()
#include <pthread.h>

#include "mot.h"
#include "motorboard.h"

//need 0xff for reliable startup, after startup min pwm 0x50 is allowed
const u16 mot_pwm_min=0x00; 
const u16 mot_pwm_max=0x1ff;

//m0=Front Left m1=Front Right m3=Rear Right m4=Rear Left
struct mot_struct
{
  float mot[4]; //motor speed setting. 0.0=min power, 1.0=full power
  u16 pwm[4];   //motor speed 0x00-0x1ff.  -- protected by mutex
  u08 led[4];   //led 0=off 1=red 2=green 3=orange -- protected by mutex
  u08 NeedToSendLedCmd;
};
pthread_t mot_thread;
pthread_mutex_t mot_mutex;
mot_struct mot;


//mot thread
void *mot_main(void* data)
{
	mot.NeedToSendLedCmd=1;
	while(1) {
		//5000us = 200Hz update interval
		usleep(5000); 

		pthread_mutex_lock(&mot_mutex);
		
		//write motor speed command
		motorboard_SetPWM(mot.pwm[0],mot.pwm[1],mot.pwm[2],mot.pwm[3]);

		if(mot.NeedToSendLedCmd) {
			//write led command
			motorboard_SetLeds(mot.led[0],mot.led[1],mot.led[2],mot.led[3]);
			mot.NeedToSendLedCmd=0;
		}

		pthread_mutex_unlock(&mot_mutex);
	}
}

int mot_Init() {
	int rc;

	rc = motorboard_Init();
	if(rc) return rc;
	
	mot.led[0]=MOT_LEDGREEN;
	mot.led[1]=MOT_LEDGREEN;
	mot.led[2]=MOT_LEDGREEN;
	mot.led[3]=MOT_LEDGREEN;
  
	//create mutex
	pthread_mutex_init(&mot_mutex, NULL);

	//start mot thread 
	rc = pthread_create(&mot_thread, NULL, mot_main, NULL); 
	if (rc) {
		printf("ERROR: Return code from pthread_create(mot_thread) is %d\n", rc);
		return 202;
	}
}

void mot_SetLed(u08 mot_id, u08 led) 
{
  mot_id &= 3;
  led &= 3;
  pthread_mutex_lock(&mot_mutex);
  if(mot.led[mot_id]!=led&3) {mot.led[mot_id]=led&3; mot.NeedToSendLedCmd=1;}
  pthread_mutex_unlock(&mot_mutex);
}

void mot_SetLeds(u08 led0, u08 led1, u08 led2, u08 led3) {
  pthread_mutex_lock(&mot_mutex);
  if(mot.led[0]!=led0&3) {mot.led[0]=led0&3; mot.NeedToSendLedCmd=1;}
  if(mot.led[1]!=led1&3) {mot.led[1]=led1&3; mot.NeedToSendLedCmd=1;}
  if(mot.led[2]!=led2&3) {mot.led[2]=led2&3; mot.NeedToSendLedCmd=1;}
  if(mot.led[3]!=led3&3) {mot.led[3]=led3&3; mot.NeedToSendLedCmd=1;}
  pthread_mutex_unlock(&mot_mutex);
}  

void mot_SetPWM(u16 m0, u16 m1, u16 m2, u16 m3) {
  pthread_mutex_lock(&mot_mutex);
  mot.pwm[0] = m0 & 0x1ff; 
  mot.pwm[1] = m1 & 0x1ff; 
  mot.pwm[2] = m2 & 0x1ff; 
  mot.pwm[3] = m3 & 0x1ff; 
  pthread_mutex_unlock(&mot_mutex);
  //printf("===SetPWM(%d,%d,%d,%d)\r\n",m0,m1,m2,m3);
}

void mot_Stop()
{
  pthread_mutex_lock(&mot_mutex);
  mot.pwm[0] = 0; 
  mot.pwm[1] = 0; 
  mot.pwm[2] = 0; 
  mot.pwm[3] = 0; 
  mot.mot[0]=0;
  mot.mot[1]=0;
  mot.mot[2]=0;
  mot.mot[3]=0;
  pthread_mutex_unlock(&mot_mutex);
}

//run motors: 0.0=minimum speed, 1.0=maximum speed (clipped to these values)
void mot_Run(float m0, float m1, float m2, float m3)
{
  mot.mot[0]=m0;
  mot.mot[1]=m1;
  mot.mot[2]=m2;
  mot.mot[3]=m3;
  
  //convert to pwm values, clipped at mot_pwm_min and mot_pwm_max
  float pwm[4];
  for(int i=0;i<4;i++) {
    if(mot.mot[i]<0.0) mot.mot[i]=0.0;
    if(mot.mot[i]>1.0) mot.mot[i]=1.0;
    pwm[i]=mot_pwm_min + mot.mot[i]*(mot_pwm_max-mot_pwm_min);
	if(pwm[i]<mot_pwm_min) pwm[i]=mot_pwm_min;
	if(pwm[i]>mot_pwm_max) pwm[i]=mot_pwm_max;
  }
    
  mot_SetPWM((u16)pwm[0],(u16)pwm[1],(u16)pwm[2],(u16)pwm[3]);
}

void mot_GetMot(float *m) 
{
	m[0]=mot.mot[0];
	m[1]=mot.mot[1];
	m[2]=mot.mot[2];
	m[3]=mot.mot[3];
}

void mot_Close()
{
  motorboard_Close();
}
