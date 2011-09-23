/*
    pid.c - pid controller

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
#include "pid.h"

void pid_Init(pid_struct *pid, float kp, float ki, float kd, float i_max) 
{
	pid->kp=kp;
	pid->ki=ki;
	pid->kd=kd;
	pid->i_max=i_max;
	pid->i=0;
	pid->e_prev=0;
}

float pid_CalcD(pid_struct *pid, float error, float dt, float d)
{
	pid->i += error * dt;
	if(pid->i > pid->i_max) pid->i = pid->i_max;
	if(pid->i < -pid->i_max) pid->i = -pid->i_max;
	float out = pid->kp * error + pid->ki * pid->i + pid->kd * d;
	pid->e_prev = error;
	return out;
}

float pid_Calc(pid_struct *pid, float error, float dt)
{
	return pid_CalcD(pid,  error, dt, (error - pid->e_prev)/dt);
}

/*
previous_error = setpoint - actual_position
integral = 0
start:
  error = setpoint - actual_position
  integral = integral + (error*dt)
  derivative = (error - previous_error)/dt
  output = (Kp*error) + (Ki*integral) + (Kd*derivative)
  previous_error = error
  wait(dt)
  goto start
*/