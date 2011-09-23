/*
    main_attitude.c - AR.Drone attitude estimate demo program

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
//#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
//#include <fcntl.h>   /* File control definitions */
//#include <errno.h>   /* Error number definitions */
//#include <termios.h> /* POSIX terminal control definitions */
//#include <stdlib.h>  //exit()
#include <pthread.h>
#include <ctype.h>    /* For tolower() function */
#include <math.h>

#include "../util/type.h"
#include "../util/util.h"
#include "attitude.h"

int main()
{
	int rc;
	att_struct att;
	
	printf("Attitude Estimate test program\r\n");

	//init att board
	printf("Init Attitute Estimate ...\r\n");
	rc = att_Init(&att);
	if(rc) return rc;
	printf("Init Attitute Estimate OK\r\n");

	//main loop	
	while(1) { 
		//get sample
		rc = att_GetSample(&att);
		if(rc) {
			printf("ERROR: att_GetSample return code=%d\n",rc); 
		}
		att_Print(&att);
	}
	att_Close();
	printf("\nDone...\n");
	return 0;
}
