/*
    util.c - utilities

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
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <time.h>  
#include <sys/time.h>

#include "util.h"

//non blocking getchar
int util_getch(void)
{
struct termios oldt,
newt;
int ch=-1;
tcgetattr( STDIN_FILENO, &oldt );
newt = oldt;
newt.c_lflag &= ~( ICANON | ECHO );
tcsetattr( STDIN_FILENO, TCSANOW, &newt );
fcntl(STDIN_FILENO, F_SETFL, FNDELAY);
ch = getchar();
fcntl(STDIN_FILENO, F_SETFL, 0);
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
return ch;
}

//return timestamp in seconds with microsecond resolution
double util_timestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL); 
  return (double)tv.tv_sec+((double)tv.tv_usec)/1000000;
}

//return timestamp in microseconds since first call to this function
int util_timestamp_int()
{
  static struct timeval tv1;
  struct timeval tv;
  if(tv1.tv_usec==0 && tv1.tv_sec==0) gettimeofday(&tv1, NULL); 
  gettimeofday(&tv, NULL); 
  return (int)(tv.tv_sec-tv1.tv_sec)*1000000+(int)(tv.tv_usec-tv1.tv_usec);
}