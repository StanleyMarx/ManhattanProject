#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_int_list(int* list, int len){
    printf("[");
    for (int i=0; i<len-1; i++){
        printf("%d,",list[i]);
    }
    printf("%d]\n",list[len-1]);
}
int pos_exists(int x, int y, int* x_list, int* y_list, int len){
    int i=0;
    while (i<len){
        if (x==x_list[i] && y==y_list[i]){
            return 1;
        }
        i++;
    }
    return 0;
}
void send_map_from_file(){
    // file: pos.txt
    // max number of coordinates: 100
    // uses send_mapdata_pos for debug pruposes but looking forward to send_mapdata
    
    int x_list[100];
    int y_list[100];
    
    FILE* pos_file=fopen("test_pos.txt","r");
    if (!pos_file){
        printf("[ERROR] print_map(): couldn't open pos.txt in read mode\n");
    }
    
    char is_x=1;
    int i_x=0;
    int i_y=0;
    char c=getc(pos_file);
    while (c!=EOF){
        switch(c){
            case ',':
                is_x=0;
                break;
            case '\n':
                is_x=1;
                break;
            default:
                if (is_x){
                    x_list[i_x]=c-48;
                    i_x++;
                } else {
                    y_list[i_y]=c-48;
                    i_y++;
                }
                break;
        }
        c=getc(pos_file);
    }
    fclose(pos_file);
    /* at this point, x_list and y_list contains the list of coordinates, and i_x is the length of these lists */
    
    // DEBUG
    printf("list of coordinates:\n");
    print_int_list(x_list,i_x);
    print_int_list(y_list,i_y);
    
    int xmin=x_list[0],xmax=x_list[0],ymin=y_list[0],ymax=y_list[0];
    for (int i=1; i<i_x; i++){
        if (x_list[i]<xmin){
            xmin=x_list[i];
        }
        if (x_list[i]>xmax){
            xmax=x_list[i];
        }
        if (y_list[i]<ymin){
            ymin=y_list[i];
        }
        if (y_list[i]>ymax){
            ymax=y_list[i];
        }
    }
    
    // DEBUG
    printf("xmin,xmax,ymin,ymax = %d,%d,%d,%d\n",xmin,xmax,ymin,ymax);
    
    printf("map:\n")
    for (int y=ymin; y<=ymax; y++){
        for (int x=xmin; x<=xmax; x++){
            if (pos_exists(x,y,x_list,y_list,i_x)){
                send_mapdata_pos(x,y,0,0,0);
                printf(" X ");
            }
            else {
                send_mapdata_pos(x,y,255,255,255);
                printf(" . ");
            }
        }
        printf("\n");
    }
    
    printf("map sent to the server\n");    
}

int main(){
    send_map_from_file();
}














