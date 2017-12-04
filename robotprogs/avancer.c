#include <stdio.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

void forward(sn_left, sn_right) {
	int max_speed;
	get_tacho_max_speed(sn_left, &max_speed );
	printf("  max speed = %d\n", max_speed );
	set_tacho_stop_action_inx( sn_left, TACHO_COAST );
	set_tacho_stop_action_inx( sn_right, TACHO_COAST );
	set_tacho_speed_sp(sn_left, max_speed * 2 / 3 );
	set_tacho_speed_sp(sn_right, max_speed * 2 / 3 );
	set_tacho_time_sp( sn_left, 5000 );
	set_tacho_time_sp( sn_right, 5000 );
	set_tacho_ramp_up_sp( sn_left, 2000 );
	set_tacho_ramp_up_sp( sn_right, 2000 );
	set_tacho_ramp_down_sp( sn_left, 2000 );
	set_tacho_ramp_down_sp( sn_right, 2000 );
	set_tacho_command_inx( sn_left, TACHO_RUN_TIMED );
	set_tacho_command_inx( sn_right, TACHO_RUN_TIMED );
	/* Wait tacho stop */
	Sleep( 100 );
}
