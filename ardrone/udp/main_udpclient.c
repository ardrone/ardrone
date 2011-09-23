#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "udp.h"

void diep(const char *s)
{
  perror(s);
  exit(1);
}

int main(void)
{
  udp_struct udp;

  int msglen;
  char buf[512];
  
  if(udpClient_Init(&udp,"127.0.0.1",9930)) diep("udpClient_Init");
  
  for (int i=0; i<10; i++) {
    printf((char*)"Sending packet %d\n", i);
    msglen=sprintf(buf, (char*)"This is packet %d\n", i);
    if (udpClient_Send(&udp, buf, msglen)) diep("send");
    usleep(100000);
  }
  
  udpClient_Close(&udp);
  return 0;
}
