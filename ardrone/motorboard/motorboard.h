/*
    motorboard.h - AR.Drone motor driver

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
#ifndef _MOTORBOARD_H
#define _MOTORBOARD_H

#include "../util/type.h"

#define MOT_LEDOFF 0
#define MOT_LEDRED 1
#define MOT_LEDGREEN 2
#define MOT_LEDORANGE 3

int motorboard_Init();
void motorboard_SetPWM(u16 pwm0, u16 pwm1, u16 pwm2, u16 pwm3);
void motorboard_SetLeds(u08 led0, u08 led1, u08 led2, u08 led3);
void motorboard_Close();

#endif