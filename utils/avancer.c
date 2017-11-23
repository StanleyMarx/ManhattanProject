/*
usage:        ./avancer [distance]
description:  fait avancer le robot d'une distance proportionnelle à la distance entrée en paramètre
*/

#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"

int main(int argc, char** argv){
    int A=1000;
    int B=0.66;
    
    if (argc!=3){
        printf("Wrong usage: param1 = dist to run\n");
    }
    int dist=atoi(argv[1]);
    int port;
    uint8_t sn;
    for (port=65; port<69; port++){
        if (ev3_search_tacho_plugged_in(port,0,&sn,0)){
            printf("Motor is found\n");
            int max_speed;
            get_tacho_max_speed(sn,&max_speed);
            printf("Found maximum speed of motors: %d\n",max_speed);
            set_tacho_stop_action_inx(sn,TACHO_COAST);
            set_tacho_speed_sp(sn,max_speed*B);
            set_tacho_time_sp(sn,dist*A);
            set_tacho_ramp_up_sp(sn,2000);
            set_tacho_ramp_down_sp(sn,2000);
            set_tacho_command_inx(sn,TACHO_RUN_TIMED);
            printf("done\n");
        }
    }
}
