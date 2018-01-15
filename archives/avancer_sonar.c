#include <stdio.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

void forwardSonar(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar) {
	int max_speed;
	size_t sonarVal;
	get_sensor_value(sn_sonar, &sonarVal);
	printf("initial sonar value : %f", sonarVal);
	get_tacho_max_speed(sn_left, &max_speed );
	printf("  max speed = %d\n", max_speed );
	set_tacho_speed_sp(sn_left, max_speed * 2 / 3 );
	set_tacho_speed_sp(sn_right, max_speed * 2 / 3 );
	while (sensorVal > 50){ 
		set_tacho_command( sn_left, TACHO_RUN_FOREVER );
		set_tacho_command( sn_right, TACHO_RUN_FOREVER );
		get_sensor_value0(sn_sonar, &sonarVal) 
		printf("no obstacle yet");
	}
	printf("obsacle near");
	set_tacho_command(sn_left, STOP);
	set_tacho_command(sn_right, STOP);
}
