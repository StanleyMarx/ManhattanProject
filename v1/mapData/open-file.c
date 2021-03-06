#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int minX = 0;
int minY = 0;
int maxX = 0;
int maxY = 0;
FILE* posFile = NULL;

void find_corners() {
	/*
		by JB
		recovers all of the past coordinates of the robots from the Position text file and deduce the possible corners of a rectangular map.
	*/
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    char *token;

  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL)
        exit(1);

    while ((read = getline(&line, &len, posFile)) != -1) {
		x = -1000;
		y = -1000;
		token = strtok(line, ",");
    	while(token) {
        	if (x==-1000) {
        		x = atoi(token);
        	} else if (y==-1000) {
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
	printf("Bottom left : %d, %d\n", minX, minY);
	printf("Bottom right: %d, %d\n", maxX, minY);
	printf("Top left    : %d, %d\n", minX, maxY);
	printf("Top right   : %d, %d\n", maxX, maxY);
	printf("\n");
}

int create_map(int x0, int y0) {
	/*
		by JB
		Given the map corners and all of the robot's past coordinates, this script creates a map of where the robot went and where it did not.
		We need to add the send_map_data_message function in here so that it sends the right map messages to the server.
	*/
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    int xFile; int yFile;
    int nature;
	int found = 0;
	char *token;
	//char map[maxY-minY][maxX-minX];
	find_corners();
  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL){
        printf("[ERROR] create_map(): couldn't open pos.txt\n");
        exit(1);
    }
    for (y=maxY; y>minY-1; y--) {
    	for (x=minX; x<maxX+1; x++) {
    		found = 0;
    		posFile = fopen("pos.txt", "r");
			while (!found && ((read = getline(&line, &len, posFile)) != -1)) {
				xFile = -1000; // value that should never be reached
				yFile = -1000; // value that should never be reached
				nature = -1;
				token = strtok(line, ",");
				while(token) {
					if (xFile==-1000) {
						xFile = atoi(token);
					} else if (yFile==-1000) {
						yFile = atoi(token);
					} else if (nature==-1) {
						nature = atoi(token);
					}
					token = strtok(NULL, ",");
		   		}
				if (x==xFile && y==yFile) {
					found = 1;
				}
				//free(line);
			}
			if (x==xFile && y==yFile){
				if (x==x0 && y==y0) {
					printf(" 0 ");
				} else if (nature==0) {
					printf(" X ");// send_mapdata_pos((int16_t) x, (int16_t) y, 255, 255, 255); //### WHITE: %d, %d\n", x, y);
				} else {
					printf(" + ");//this is an object
				}
			} else {
				printf(" . ");// send_mapdata_pos((int16_t) x, (int16_t) y, 0, 0, 0); //BLACK: %d, %d\n", x, y);
			}
			fclose(posFile);
		}
		printf("\n");
	}
	printf("\n");
    return 0;
}

int append_pos_file(int x, int y, int nature) {
	/*
		by JB and Alix
		When the robot is at (x,y), this script writes the coordinates in the Position text file that will be used to build the map at the end of the exploration. 
		nature = 0 is it is robot position, 1 if it is an object position
	*/
   	posFile = fopen("pos.txt", "a");
    if (posFile != NULL){
    	fprintf(posFile, "%d,%d,%d\n",x,y,nature);
        fclose(posFile);
    }
    else {
        printf("[ERROR] append_pos_file(%d,%d): couldn't open pos.txt",x,y);
    }
    return 0;
}
/*
int known_point(int checkX, int checkY) {
	/
		by JB
		checks if the robot already went through this (x, y) coordinates.
		returns 1 if coordinates in file already, 0 otherwise.
	/
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    int nature;
    char *token;
    int count=0;

  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL) exit(1);

    while ((read = getline(&line, &len, posFile)) != -1) {
  		x = -1000;
  		y = -1000;
  		nature = -1;
  		token = strtok(line, ",");
    	while(token) {
        	if (x==-1000) {
        		x = atoi(token);
        	} else if (y==-1000) {
        		y = atoi(token);
        	} else if (nature==-1) {
        		nature = atoi(token);
        	}
        	token = strtok(NULL, ",");
   		}
      if (x==checkX && y==checkY && count>0) return 1;
      if (x==checkX && y==checkY) count++;
    }
    fclose(posFile);
    if (line) free(line);
  	return 0;
}*/

///////////////////////////////////////////////////////////

int fill_obstacles() {
	/*
		by JB
	*/
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int x; int y;
	char *token;
	posFile = fopen("pos.txt", "r");
	if (posFile == NULL){
		printf("[ERROR] create_map(): couldn't open pos.txt\n");
		exit(1);
	}
	while ((read = getline(&line, &len, posFile)) != -1) {
		x = -1000; // value that should never be reached
		y = -1000; // value that should never be reached
		token = strtok(line, ",");
		while(token) {
			if (x==-1000) {
				x = atoi(token);
			} else if (y==-1000) {
				y = atoi(token);
			}
			token = strtok(NULL, ",");
		}
		printf("x=%d, y=%d\n", x, y);
	}
	fclose(posFile);
	return 0;
}


///////////////////////////////////////

int main() {
	/*
		by JB
		Just a main to test the different functions that interact with the Position text file.
	*/
	posFile = fopen("pos.txt", "w");
	fclose(posFile);

	append_pos_file(0,0,0);
	append_pos_file(0,1,0);
	append_pos_file(0,2,0);
	append_pos_file(1,2,0);
	append_pos_file(2,2,0);
  // around square
	append_pos_file(2,3,0);
	append_pos_file(2,4,0);
	append_pos_file(2,5,0);

	append_pos_file(3,5,0);
	append_pos_file(4,5,0);
	append_pos_file(5,5,0);
	append_pos_file(6,5,0);

	append_pos_file(6,4,0);
	append_pos_file(6,3,0);
	append_pos_file(6,2,0);
	append_pos_file(6,1,0);

	append_pos_file(5,1,0);
	append_pos_file(4,1,0);
	append_pos_file(3,1,0);
	append_pos_file(2,1,0);

	append_pos_file(3,2,1);
	/*int res;
	res = known_point(2, 4);
	printf("%d should be equal to 0\n", res);
	res = known_point(2, 2);
	printf("%d should be equal to 1\n", res);
	fill_obstacles();*/
	create_map(0,0);
	exit(0);
}
