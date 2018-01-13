#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_int_list(int* list, int len){
    printf("[")
    for (int i; i<len-1; i++){
        printf("%d,",list[i]);
    }
    printf("%d]\n",list[len-1]);
}

int print_map(){
    int x_list[100];
    int y_list[100];
    
    FILE* pos_file=fopen("pos.txt","r");
    if (!pos_file){
        printf("[ERROR] print_map(): couldn't open pos.txt in read mode\n");
    }
    
    char is_x=0;
    int i_x=0;
    int i_y=0;
    char c=getc(pos_file);
    while (c!=EOF){
        switch(c){
            case ',':
                break;
            case '\n':
                break;
            default:
                if (is_x){
                    x_list[i_x]=(int)c;
                } else {
                    y_list[i_y]=(int)c;
                }
                break;
        }
        c=getc(pos_file);
    }
    
}

int main(){
    FILE* pos=fopen("test.txt","r");
    char c=getc(pos);
    while (c!=EOF){
        printf("found a char: %c\n",c);
        c=getc(pos);
    }
    printf("-end fo file-\n");
}