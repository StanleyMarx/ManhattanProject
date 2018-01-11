#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <math.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <math.h>
#include <pthread.h>

#define SERV_ADDR "30:3a:64:ea:bf:0e" // server = nino-hp

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

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"

#ifdef __WIN32__
#include <windows.h>
#else
#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )
#endif

#include "myev3.h"
#include "main-prog.h"

int main(int argc, char **argv) {
    
    while(ev3_tacho_init()<1) Sleep( 1000 );
    ev3_sensor_init();
    ev3_search_sensor(HT_NXT_COMPASS,&sn_compass,0);
    ev3_search_sensor(LEGO_EV3_GYRO, &sn_gyro,0);
    ev3_search_sensor(LEGO_EV3_US,&sn_sonar,0);
    //ev3_search_sensor(LEGO_EV3_TOUCH,&sn_touch,0);
 	ev3_search_sensor(LEGO_EV3_COLOR,&sn_color,0);
   	set_sensor_mode(sn_color, "COL-COLOR");
    ev3_search_tacho_plugged_in(PORT_RIGHTWHEEL,0,&sn_rwheel,0);
    ev3_search_tacho_plugged_in(PORT_SHOVEL,0,&sn_shovel,0);
    ev3_search_tacho_plugged_in(PORT_LEFTWHEEL,0,&sn_lwheel,0);
    get_tacho_max_speed(sn_rwheel,&max_speed);

	robot(atoi(argv[1]));
	
	return 0;
}
