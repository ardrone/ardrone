/*
    util.h - utilities

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
#ifndef _UTIL_H
#define _UTIL_H

//degrees to radians
#define DEG2RAD(x) ((x)*3.1415926/180)
//radians to degrees
#define RAD2DEG(x) ((x)/3.1415926*180)

//non blocking getchar
int util_getch(void);
//return timestamp in seconds with microsecond resolution
double util_timestamp();
int util_timestamp_int();
#endif