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
  
  if(udpServer_Init(&udp,9930, 0)) diep("udpServer_Init");
  
  for (int i=0; i<10; i++) {
    int cnt=0;
    do {
      msglen = udpServer_Receive(&udp, buf, 512);
      cnt++;
    } while(msglen<=0);
    printf("Received at cnt=%d -> '%s'\n\n", cnt, buf);
  }
  
  udpClient_Close(&udp);
  return 0;
}
