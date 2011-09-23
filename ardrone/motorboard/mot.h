/*
    mot.h - AR.Drone motor thread

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
#ifndef _MOT_H
#define _MOT_H

#include "../util/type.h"
#include "motorboard.h"

int mot_Init();
void mot_SetLed(u08 mot_id, u08 led);
void mot_SetLeds(u08 led0, u08 led1, u08 led2, u08 led3);
void mot_Stop();
void mot_Run(float m0, float m1, float m2, float m3);
void mot_GetMot(float *m);
void mot_Close();

#endif