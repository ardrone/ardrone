/*
    main_vbat.c - AR.Drone battery voltage monitor demo program

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
#include <unistd.h>
#include "vbat.h"

int main(int argc, char *argv[]) {
	vbat_struct vbat;

	if(vbat_init(&vbat)) return 1;

	printf("Setpoints:  Vdd0=%4.2fV Vdd1=%4.2fV Vdd2=%4.2fV Vdd3=%4.2fV Vdd4=%4.2fV\n",vbat.vdd0_setpoint,vbat.vdd1_setpoint,vbat.vdd2_setpoint,vbat.vdd3_setpoint,vbat.vdd4_setpoint);
	printf("==================================================================\n");

	while(1) {
		vbat_read(&vbat);
		printf("Vbat=%5.2fV Vdd0=%4.2fV Vdd1=%4.2fV Vdd2=%4.2fV Vdd3=%4.2fV Vdd4=%4.2fV\n",vbat.vbat,vbat.vdd0,vbat.vdd1,vbat.vdd2,vbat.vdd3,vbat.vdd4);
		usleep(100000);
	}

	return 0;
}
