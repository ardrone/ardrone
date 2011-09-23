/*
    navboard.h - AR.Drone navboard driver

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
#ifndef _NAVBOARD_H
#define _NAVBOARD_H
#include "../util/type.h"

///////////////////////////////////////////////////////////////////////////////
// nav_struct
///////////////////////////////////////////////////////////////////////////////
/*
Microchip PIC24HJ16GP304 
12 bit ADC, Vref=3.3V -> 0.806mV/lsb

IDG-500 (Dual-axis gyroscope) 
X-/Y-Out Pins: 500°/s full scale range, 2.0mV/°/s sensitivity
X/Y4.5Out Pins: 110°/s full scale range, 9.1mV/°/s sensitivity
Vref = 1350±50mV
Temperature Sensor: Range -20 to +85°C, Sensitivity 4mV/°C, Offset 1.25V at room temperature

Epson XV-3500CB (Z-axis gyroscope) 
0.67mV/deg/s  Vref=1350+/-30mV

Bosch BMA150 (3-axis accelerometer) 
i2c interface
Acceleration Sensor: 10bit resolution, range -2 to 2G 
Temperature Sensor: 8bit, 0.5°C/lsb, range -30 to 97.5°C
*/
struct nav_struct
{
	u16 size;                // +0x00 Size of the following data (always 0x2C) 
	u16 seq;                 // +0x02 Sequence number, increases every update 
	u16 acc[3];              // +0x04 Raw data (10-bit) of the accelerometers multiplied by 4 
	u16 gyro[3];             // +0x0A Raw data for the gyros, 12-bit A/D converted voltage of the gyros. X,Y=IDG, Z=Epson  
	u16 gyro_110[2];         // +0x10 4.5x Raw data (IDG), gyro values with another resolution (see IDG-500 datasheet)  
	u16 acc_temp;            // +0x14 Accs temperature -- startup value 120 @ 25C, rising to 143
	u16 gyro_temp;           // +0x16 XYGyro temperature (IDG), 12-bit A/D converted voltage of IDG's temperature sensor -- startup value 1532 @ 25C, rising to 1572
	u16 vrefEpson;           // +0x18 ZGyro v_ref (Epson), 12-bit A/D converted reference voltage of the Epson sensor  
	u16 vrefIDG;             // +0x1A XYGyro v_ref (IDG), 12-bit A/D converted reference voltage of the IDG sensor   
	u16 us_echo;             // +0x1C bit15=1 echo pulse transmitted, bit14-0 first echo. Value 30 = 1cm. min value: 784 = 26cm
	u16 checksum;            // +0x1E Checksum = sum of all values except checksum (22 values)
	u16 us_echo_start;       // +0x20 Array with starts of echos (8 array values @ 25Hz, 9 values @ 22.22Hz)
	u16 us_echo_end;         // +0x22 Array with ends of echos   (8 array values @ 25Hz, 9 values @ 22.22Hz)
	u16 us_association_echo; // +0x24 Ultrasonic parameter -- echo number starting with 0. max value 3758. examples: 0,1,2,3,4,5,6,7  ; 0,1,2,3,4,86,6,9
	u16 us_distance_echo;    // +0x26 Ultrasonic parameter -- no clear pattern
	u16 us_courbe_temps;     // +0x28 Ultrasonic parameter -- counts up from 0 to approx 24346 in 192 sample cycles of which 12 cylces have value 0
	u16 us_courbe_valeur;    // +0x2A Ultrasonic parameter -- value between 0 and 4000, no clear pattern. 192 sample cycles of which 12 cylces have value 0
	u16 us_courbe_ref;       // +0x2C Ultrasonic parameter -- coutns down from 4000 to 0 in 192 sample cycles of which 12 cylces have value 0
	//end of data received from nav board  
	double ts; //timestamp in seconds with microsecond resolution
	float dt; //time since last sample
	float ax;   // acceleration x-axis in [G] front facing up is positive         
	float ay;   // acceleration y-axis in [G] left facing up is positive                
	float az;   // acceleration z-axis in [G] top facing up is positive             
	float gx;   // gyro value x-axis in [rad/sec] right turn, i.e. roll right is positive           
	float gy;   // gyro value y-axis in [rad/sec] right turn, i.e. pirch down is positive                     
	float gz;   // gyro value z-axis in [rad/sec] right turn, i.e. yaw left is positive                           
	float t;    // temperature in [C] 
	float h;    // height above ground in [cm] 
  char h_meas;// 1 if this is a new h measurement, 0 otherwise
	float ta;   // temperature acc
	float tg;   // temperature gyro  
};

int nav_Init(nav_struct* nav);
int nav_FlatTrim();
int nav_GetSample(nav_struct* nav);
void nav_Print(nav_struct* nav);
void nav_Close();

#endif