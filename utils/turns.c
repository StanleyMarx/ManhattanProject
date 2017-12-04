#include ‹stdio.h›
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"

void simpleTurn(int speed, int angle) {

        printf("-----simpleTurn-----\n");

        int pos = abs(angle)*(5.8)/(2.8);
        ev3_set_speed_sp(motor1, 300);
        ev3_set_speed_sp(motor2, 300);
        ev3_set_position_sp(motor1, sign(angle)*pos);
        ev3_set_position_sp(motor2, -(sign(angle)*pos));

        ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        ev3_command_motor_by_name(motor2, "run-to-rel-pos");
        while (
                (ev3_motor_state(motor1) !=5) || (ev3_motor_state(motor2) !=5)
        );
        usleep(50000);
}

void turnLeft() {
	simpleTurn(50, -90);
}

void turnRight() {
	simpleTurn(50, 90);
}
