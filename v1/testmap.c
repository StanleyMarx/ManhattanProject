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
int is_in_list(int elt, int* list, int len){
    int i=0;
    while (i<len){
        if (elt==list[i]){
            return 1;
        }
        i++;
    }
    return 0;
}

int print_map(){    
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
    /* at this point, x_list and y_list contains the list of coordinates, and i_x is the length of these lists */
    
    // DEBUG
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
    
    for (int x=xmin; x<=xmax; x++){
        for (int y=ymin; y<=ymax; y++){
            if (is_in_list(x,x_list,i_x)&&is_in_list(y,y_list,i_y)){
                printf("");
            }
            else {
            }
        }
        printf("\n");
    }
    
    
}

int main(){
    print_map();
}














