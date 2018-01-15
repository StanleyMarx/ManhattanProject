int add_pos_fic(int x, int y){
	char z[8];
	z[8] = '\0';
	z[
	char newx = (char) x;
	char newy = (char) y ;
	if (srlen(newx) == 1){
		z[0] = '0';
		z[1] = '0';
		z[2] = newx;	
	}
	elif (srlen(newx) == 2){
		z[0] = '0';
		z[1] = newx[0];
		z[2] = newx[1];
	}
}
