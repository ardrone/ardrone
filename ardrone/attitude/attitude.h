/*
    attitude.h - AR.Drone attitude estimate driver

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
#ifndef _ATTITUDE_H
#define _ATTITUDE_H

struct att_struct {
  //pitch estimates in radians, positive is pitch down (fly forward)
  float pitch_g;  //=sum(gx * dt)
  float pitch_a;  //=pitch(az,ax)
  float pitch;    //kalman pitch estimate from gy and pitch_a

  //roll estimates in radians, positive is roll right (fly rightward)
  float roll_g;   //=sum(gy * dt)
  float roll_a;   //=roll(az,ay)
  float roll;     //kalman roll estimate from gx and roll_a
  
  //yaw estimate, positive is yaw left
  float yaw;      //=sum(gz * dt)

  //height and speed estimate
  float h;    //smoothend hraw
  float hv;   //vertical speed in cm/sec
  
  double dt;  // time since last navdata sample in sec
  
  //copy of physical navdata values
  double ts;  // navdata timestamp in sec
  float hraw;    // height above ground in [cm] 
  char h_meas;// 1=height was measured in this sample, 0=height is copy of prev value
  float ax;   // acceleration x-axis in [G] front facing up is positive         
	float ay;   // acceleration y-axis in [G] left facing up is positive                
	float az;   // acceleration z-axis in [G] top facing up is positive             
	float gx;   // gyro value x-axis in [rad/sec] right turn, i.e. roll right is positive           
	float gy;   // gyro value y-axis in [rad/sec] right turn, i.e. pirch down is positive                     
	float gz;   // gyro value z-axis in [rad/sec] right turn, i.e. yaw left is positive 
};

int att_Init(att_struct *att);
int att_Init(att_struct *att);
int att_GetSample(att_struct *att);
int att_FlatTrim(att_struct *att); //recalibrate
void att_Print(att_struct *att);
void att_Close();
#endif