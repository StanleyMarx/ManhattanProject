void send_position(int16_t x,int16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=MSG_POSITION;
	*((int16_t*)&str[5])=x;
	*((int16_t*)&str[7])=y;
	write(s,str,9);
	Sleep(1000);
}
void send_mapdata(int16_t x,int16_t y,char r,char g,char b){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=5;
	*((int16_t*)&str[5])=x;	
	*((int16_t*)&str[7])=y;
	str[9]=r;
	str[10]=g;
	str[11]=b;
	write(s,str,12);
	Sleep(1000);
}
void send_mapdone(){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=6;
	write(s,str,5);
	Sleep(1000);
}
void send_obstacle(int act,uint16_t x,uint16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=7;
    str[5]=act;
	*((int16_t*)&str[6])=x;	
	*((int16_t*)&str[8])=y;
	write(s,str,10);
	Sleep(1000);
}

void send_position_pos(int16_t x,int16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=MSG_POSITION;
    str[5]=x;
    str[6]=0x00;
    str[7]=y;
    str[8]=0x00;
	write(s,str,9);
	Sleep(1000);
}
void send_mapdata_pos(int16_t x,int16_t y,char r,char g,char b){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=5;
    str[5]=x;
    str[6]=0x00;
    str[7]=y;
    str[8]=0x00;
	str[9]=r;
	str[10]=g;
	str[11]=b;
	write(s,str,12);
	Sleep(100);
}
void send_obstacle_pos(int act,uint16_t x,uint16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=7;
    str[5]=act;
    str[6]=x;
    str[7]=0x00;
    str[8]=y;
    str[9]=0x00;
	write(s,str,10);
	Sleep(1000);
}

void robot_test(int s,uint16_t msgId){
    char str[58];
    int type;
	printf("[ROBOT] ---begin---\n");
    
	printf("[ROBOT] sending position (1,2)...\n");
    send_position_pos(1,2);
    printf("[ROBOT] picking up an obstacle at (3,4)...\n");
    send_obstacle_pos(1,3,4);
    printf("[ROBOT] dropping an obstacle at (5,6)...\n");
    send_obstacle_pos(0,5,6);
    printf("[ROBOT] sending map...\n");
    int x,y;
    for (x=0; x<5; x++){
        for (y=0; y<10; y++){
            if (x==2 && y==3){
                send_mapdata_pos(x,y,255,0,0);
            } else {
                send_mapdata_pos(x,y,0,255,255);
            }
        }
    }
    printf("[ROBOT] sending mapdone...\n");
    send_mapdone();
    printf("[ROBOT] waiting for the stop message...\n");
    while(1){
        read_from_server(s,str,58);
        type=str[4];
        if (type==MSG_STOP){
            printf("[ROBOT] received stop message - exit(success)\n");
            return;
        }
    }
}
void robot_eval(int s,uint16_t msgId){
	char str[58];
    int type;
	printf("[ROBOT] ---begin---\n");
	printf("[ROBOT] sending position (0,0)...\n");
    send_position(0,0);
    printf("[ROBOT] waiting for the stop message...\n");
    while(1){
        read_from_server(s,str,58);
        type=str[4];
        if (type==MSG_STOP){
            printf("[ROBOT] received stop message - exit(success)\n");
            return;
        }
    }
}
void robot_eval_debug(int s,uint16_t msgId){
	char str[58];
    int type;
	printf("[ROBOT] ---begin---\n");
	printf("[ROBOT] sending position (0,0)...\n");
    
    send_position_pos(0,0);
    
    printf("[ROBOT] waiting for the stop message...\n");
    while(1){
        read_from_server(s,str,58);
        type=str[4];
        if (type==MSG_STOP){
            printf("[ROBOT] received stop message - exit(success)\n");
            return;
        }
    }    
}
void robot_t2(int s,uint16_t msgId){
	char str[58];
    int type;
	printf("[ROBOT] ---begin---\n");
    
    /***/
	printf("[ROBOT] sending position (0,0)...\n");
    send_position(0,0);
    /* AVANCE DE 10CM */
	printf("[ROBOT] sending position (1,0)...\n");
    send_position(1,0);
    /***/
    
    printf("[ROBOT] waiting for the stop message...\n");
    while(1){
        read_from_server(s,str,58);
        type=str[4];
        if (type==MSG_STOP){
            printf("[ROBOT] received stop message - exit(success)\n");
            return;
        }
    }
}
