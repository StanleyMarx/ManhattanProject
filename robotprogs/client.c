#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <math.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define SERV_ADDR "dc:53:60:ad:61:90"
#define TEAM_ID 12

#define MSG_ACK 0
#define MSG_START 1
#define MSG_STOP 2
#define MSG_KICK 3
#define MSG_POSITION 4
#define MSG_MAPDATA 5
#define MSG_MAPDONE 6
#define Sleep( msec ) usleep(( msec ) * 1000 )

int s;
uint16_t msgId=0;

void debug (const char *fmt, ...) {
  va_list argp;
  va_start (argp, fmt);
  vprintf (fmt, argp);
  va_end (argp);
}

int read_from_server (int sock, char *buffer, size_t maxSize) {
  int bytes_read = read (sock, buffer, maxSize);
  if (bytes_read <= 0) {
    fprintf (stderr, "Server unexpectedly closed connection...\n");
    close (s);
    exit (EXIT_FAILURE);
  }
  printf ("[DEBUG] received %d bytes\n", bytes_read);
  return bytes_read;
}

#include "robot.h"

int main(int argc, char **argv) {
    if (argc!=2){
        printf("usage: ./client <mode>\n\t<mode=0> tests several message types and sends an imaginary evolution\n\t<mode=1> = performs the task asked for the 18th of december's 'test #2'\n");
    }
    
    
  struct sockaddr_rc addr = { 0 };
  int status;

  /* allocate a socket */
  s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  /* set the connection parameters (who to connect to) */
  addr.rc_family = AF_BLUETOOTH;
  addr.rc_channel = (uint8_t) 1;
  str2ba (SERV_ADDR, &addr.rc_bdaddr);

  /* connect to server */
  status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

  /* if connected */
  if( status == 0 ) {
    char string[58];

    /* Wait for START message */
    read_from_server (s, string, 9);
    if (string[4] == MSG_START) {
      printf ("Received start message!\n");
    }
      
        /* ROBOT */
        switch(atoi(argv[1])){
            case 0:
                robot_test(s,msgId);
                break;
            case 1:
                robot_eval(s,msgId);
                break;
        }
      
      
    close (s);

    sleep (5);

  } else {
    fprintf (stderr, "Failed to connect to server...\n");
    sleep (2);
    exit (EXIT_FAILURE);
  }

  close(s);
  return 0;
}



/*
  by Nino
  compile with ~/myOS/compile.sh
  needs robot.h to work
*/
  
