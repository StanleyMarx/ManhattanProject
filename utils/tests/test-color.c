#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif
const char const *color[] = { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };
#define COLOR_COUNT  (( int )( sizeof( color ) / sizeof( color[ 0 ])))

int main( void )
{
	int i;
	//uint8_t sn;
	//FLAGS_T state;
	//uint8_t sn_touch;
	//uint8_t sn_color;
	//uint8_t sn_compass;
	uint8_t sn_color;
	//uint8_t sn_mag;
	char s[ 256 ];
	int val;
	float value;
	uint32_t n, ii;
#ifndef __ARM_ARCH_4T__
	/* Disable auto-detection of the brick (you have to set the correct address below) */
	ev3_brick_addr = "192.168.0.204";

#endif
	if ( ev3_init() == -1 ) return ( 1 );
	
//Run all sensors
	ev3_sensor_init();
	
	printf( "Found sensors:\n" );
	for ( i = 0; i < DESC_LIMIT; i++ ) {
		if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
			printf( "  type = %s\n", ev3_sensor_type( ev3_sensor[ i ].type_inx ));
			printf( "  port = %s\n", ev3_sensor_port_name( i, s ));
			if ( get_sensor_mode( i, s, sizeof( s ))) {
				printf( "  mode = %s\n", s );
			}
			if ( get_sensor_num_values( i, &n )) {
				for ( ii = 0; ii < n; ii++ ) {
					if ( get_sensor_value( ii, i, &val )) {
						printf( "  value%d = %d\n", ii, val );
					}
				}
			}
		}
	}
	for ( ; ; ){
		if (ev3_search_sensor(LEGO_EV3_COLOR, &sn_color,0)){
			set_sensor_mode(sn_color, 'COL-COLOR');
			printf("COLOR sensor found, reading color...\n");
			if ( !get_sensor_value0(sn_color, &value )) {
				value = 0;
			}
			printf("0:none, 1:black, 2:blue, 3:green, 4:yellow, 5:red, 6:white, 7:brown\n");
			printf( "\f(%d) \n", val);
			printf( "\r(%s) \n", color[ val ]);
			fflush( stdout );
			sleep(3);
	    	}
    }

	ev3_uninit();
	printf( "*** ( EV3 ) Bye! ***\n" );

	return ( 0 );
}
