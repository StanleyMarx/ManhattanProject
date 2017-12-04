#include ‹stdio.h›
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"

void getGyroVal(uint8_t sn_gyro, float *buf) {
	while ( !get_sensor_value0(sn_gyro, &buf )) {
		value = 0;
		printf("looking for gyro value");
	}
}
