/*
    main_fly.c - AR.Drone fly demo program

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
#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
//#include <fcntl.h>   /* File control definitions */
//#include <errno.h>   /* Error number definitions */
//#include <termios.h> /* POSIX terminal control definitions */
//#include <stdlib.h>  //exit()
#include <pthread.h>
#include <ctype.h>    /* For tolower() function */
#include <math.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../util/type.h"
#include "../util/util.h"
#include "../motorboard/mot.h"
#include "../udp/udp.h"
#include "controlthread.h"

int main()
{
  printf("'fly' version 1.00 - Copyright (C) 2011 Hugo Perquin - http://blog.perquin.com\n");
  //wait for udp packet on port 7777
  udp_struct udpCmd;
  udpServer_Init(&udpCmd,7777,1/*blocking*/);
  char buf[1024];
  printf("Waiting for UDP wakeup on port 7777\n");
  int bufcnt=udpServer_Receive(&udpCmd, buf, 1024);
  if(bufcnt<=0) return 1;
  buf[bufcnt]=0;
  printf("UDP wakeup received from %s\n",inet_ntoa(udpCmd.si_other.sin_addr));

  //kill program.elf
  int rc = system("/usr/bin/killall program.elf > /dev/null 2>&1");
  printf("killall program.elf -> returncode=%d  (0=killed,256=not found)\n",rc);	
  
  //init controller
  ctl_Init(inet_ntoa(udpCmd.si_other.sin_addr));
  printf("ctl_Init completed\n");

  //main loop	
  while(1) { 
    //wait for next packet on cmd port
    bufcnt=udpServer_Receive(&udpCmd, buf, 1024);
    if(bufcnt<=0) continue;
    buf[bufcnt]=0;
    
    //split tokens
    int i=0;
    char delims[] = ",";
    char *result = NULL;
    result = strtok( buf, delims );
    if(strcmp(result,"s")) continue;
    result = strtok( NULL, delims );
    float val[4];
    while( i<4 && result != NULL ) {
      val[i]=atof(result);
      //printf( "->token%d is \"%s\" %f\n", i, result, val[i] );
      result = strtok( NULL, delims );
      i++;
    }
    if(i==4) {
      printf("set:%f,%f,%f,%f\n", val[0],val[1],val[2],val[3] );
      ctl_SetSetpoint(val[0],val[1],val[2],val[3]);
    }

  }
  ctl_Close();
  printf("\nDone...\n");
  return 0;

}
