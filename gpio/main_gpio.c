/*
    main_gpio.c - AR.Drone gpio demo program

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
#include <stdio.h>
#include <ctype.h>    /* For tolower() function */
#include "gpio.h"
#include "../util/util.h"

int main(int argc, char *argv[]) {
	printf("Main Led:  a=loff  s=green  d=orange  f=red     q=quit\r\n");
	while(1) { 
		//handle user input
		int c=tolower(util_getch());
		if(c=='a') break;
		if(c=='a') {gpio_set(63,0);gpio_set(64,0);}
		if(c=='s') {gpio_set(63,0);gpio_set(64,1);}
		if(c=='d') {gpio_set(63,1);gpio_set(64,1);}
		if(c=='f') {gpio_set(63,1);gpio_set(64,0);}
	}	

	return 0;
}
