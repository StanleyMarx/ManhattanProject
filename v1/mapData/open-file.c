#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int minX = 0; 
int minY = 0;
int maxX = 0;
int maxY = 0;
FILE* posFile = NULL;

void find_corners() {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    char *token;

  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL)
        exit(1);

    while ((read = getline(&line, &len, posFile)) != -1) {
		x = -1;
		y = -1;
		token = strtok(line, ",");
    	while(token) {
        	if (x==-1) {
        		x = atoi(token);
        	} else if (y==-1) {
        		y = atoi(token);
        	}
        	token = strtok(NULL, ",");
   		}
   		if (x<minX) minX=x;
		if (x>maxX) maxX=x;
		if (y<minY) minY=y;
		if (y>maxY) maxY=y;	
    }
    fclose(posFile);
    if (line) free(line);
}

int create_map() {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    int xFile; int yFile;
	int found = 0;
	char *token;
	//char map[maxY-minY][maxX-minX];
	
  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL) exit(1);
    for (y=maxY; y>minY-1; y--) {
    	for (x=minX; x<maxX+1; x++) { 
    		found = 0;
    		posFile = fopen("pos.txt", "r");
			while (!found && ((read = getline(&line, &len, posFile)) != -1)) {
				xFile = -1;
				yFile = -1;
				token = strtok(line, ",");
				while(token) {
					if (xFile==-1) {
						xFile = atoi(token);
					} else if (yFile==-1) {
						yFile = atoi(token);
					}
					token = strtok(NULL, ",");
		   		}
				if (x==xFile && y==yFile) {
					found = 1;
				}
				//free(line);
			}
			if (x==xFile && y==yFile){
				printf(" X ");//### WHITE: %d, %d\n", x, y);
			} else {
				printf(" . ");//BLACK: %d, %d\n", x, y);
			}
			fclose(posFile);
		}
		printf("\n");
	}
	printf("\n");
    return 0;
}

int append_pos_file(int x, int y) {
   	posFile = fopen("pos.txt", "a");
    if (posFile != NULL){
    	fprintf(posFile, "%d,%d\n",x,y);
        fclose(posFile);
    }
    return 0;
}

int main() {
	posFile = fopen("pos.txt", "w");
	fclose(posFile);
	
	append_pos_file(0,0);
	append_pos_file(1,1);
	append_pos_file(2,2);
	append_pos_file(3,3);
	append_pos_file(4,4);
	append_pos_file(1,0);
	
	find_corners();
	printf("Bottom left : %d, %d\n", minX, minY);
    printf("Bottom right: %d, %d\n", maxX, minY);
    printf("Top left    : %d, %d\n", minX, maxY);
    printf("Top right   : %d, %d\n", maxX, maxY);
    printf("\n");
    create_map();
    exit(0);
}
