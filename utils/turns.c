#include ‹stdio.h›
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

void turnRight(sn_left, sn_right, sn_gyro) {
        size_t gyroVal;
	get_sensor_value0(sn_gyro, &gyroVal);
	printf("initial gyro value: %f", gyroVal);
	int pos = (5.8)/(2.8); // tiny rotation - see if we can change this value (or actually calculate it?)
	while (gyroVal < 90) {
		/*ev3_set_speed_sp(sn_left, 300);
		ev3_set_speed_sp(sn_right, 300);
		ev3_set_position_sp(sn_left, pos);
		ev3_set_position_sp(sn_right, -pos));*/
		ev3_set_speed_sp(sn_right, -50);
		ev3_set_speed_sp(sn_left, 50);
		set_tacho_command(sn_left, TACHO_RUN_FOREVER);
		set_tacho_command(sn_right, TACHO_RUN_FOREVER);
		get_sensor_value0(sn_gyro, &gyroVal);
        	/*ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        	ev3_command_motor_by_name(motor2, "run-to-rel-pos");*/
	}
	set_tacho_command(sn_left, STOP);
	set_tacho_command(sn_right, STOP);
}

void turnLeft(sn_left, sn_right, sn_gyro) {
	size_t gyroVal;
	get_sensor_value0(sn_gyro, &gyroVal);
	printf("initial gyro value: %f", gyroVal);
	int pos = (5.8)/(2.8); // tiny rotation - see if we can change this value (or actually calculate it?)
	while (gyroVal < 90) {
		/*ev3_set_speed_sp(sn_left, 300);
		ev3_set_speed_sp(sn_right, 300);
		ev3_set_position_sp(sn_left, pos);
		ev3_set_position_sp(sn_right, -pos));*/
		ev3_set_speed_sp(sn_right, 50);
		ev3_set_speed_sp(sn_left, -50);
		set_tacho_command(sn_left, TACHO_RUN_FOREVER);
		set_tacho_command(sn_right, TACHO_RUN_FOREVER);
		get_sensor_value0(sn_gyro, &gyroVal);
        	/*ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        	ev3_command_motor_by_name(motor2, "run-to-rel-pos");*/
	}
	set_tacho_command(sn_left, STOP);
	set_tacho_command(sn_right, STOP);
}
