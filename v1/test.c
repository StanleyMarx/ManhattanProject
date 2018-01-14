#include <stdio.h>
#include <time.h>

const int CHECK_TIMER = 3;
const float SONAR_THRESHOLD = 50.0;

// SIGNATURES
int forward_timed();
int forward_while_checking_left();
int go_around_map();
int isThereSomethingInFront();
int forward_sonar_jb();
int checkSides();

// GO AROUND MAP
int forward_timed() {
	/*
		robot goes forward until:
			1/ timer runs out (CHECK_TIMER seconds)
			2/ obstacle in front of robot
	*/
	clock_t start_t, check_t;
	start_t = clock();
	int timeIsUp = 0;
	int obstacleInFront = 0;
	move_forever(50, 50);
	while (!timeIsUp && !obstacleInFront) {
		check_t = clock();
		timeIsUp = ( ((check_t - start_t) / CLOCKS_PER_SEC) > CHECK_TIMER); // should we use another timer ?
		obstacleInFront = isThereSomethingInFront();
	}
	move_forever(0,0);
	if (timeIsUp) return 0;
	return 1; // there is something in front of the robot
}

int forward_while_checking_left() {
	/*
		robot goes forward for a specific amount of time (CHECK_TIMER) then checks its left side.
	*/
	//int blocked = 0;
	int obstacleInFront = 0;
	int obstacleLeft = 0;
	while (1) { //!blocked) {
		obstacleInFront = forward_timed(); // ARGUMENTS
		turn_approx(90);
		obstacleLeft = isThereSomethingInFront();
		turn_approx(-90);
		if (!obstacleLeft) {
			return 2; // left is free
		} else if (obstacleInFront) {
			return 1; // obstacle in front
		}
		//blocked = (obstacleInFront && obstacleLeft);
	}
	//return 1;
}

int go_around_map() {
	/*
		while the robot is not back in the start area (y = 0), it keeps on going around the map starting in the bottom left corner.
		To do so, it always tries to go to the left, forward otherwise (and right if both front and left are blocked).
	*/
	turn_left();
	forward_sonar_jb();
	turn_left();
	forward_sonar_jb();
	int yPos = get_Y_position();
	int obstacleDir;
	while (yPos!=0) {
		obstacleDir = forward_while_checking_left();
		if (obstacleDir==2) turn_left();
		if (obstacleDir==1) turn_right();
	}
	int x = get_X_position();
	return x;
}

// USEFUL FUNCTIONS
int isThereSomethingInFront() {
	/*
		by JB
		returns 1 if an obstacle is close enough to the sonar sensor, 0 otherwise
	*/
	float sonarVal;
	sonarVal = get_sonar();
	return (sonarVal < SONAR_THRESHOLD);
}

int forward_sonar_jb() {
	/*
		return 0 if obstacle in front
		return 1 if obstacle to the right/left
	*/
	clock_t start_t, check_t;
	start_t = clock();
	check_t = clock();
	int timeIsUp = 0;
	int frontClear = 1;
	int sidesClear = 1;
	int sidesCheck;
	move_forever(50, 50);
	while (frontClear && sidesClear) {
		timeIsUp = (((check_t - start_t) / CLOCKS_PER_SEC) > CHECK_TIMER);
		if (timeIsUp) {
			sidesCheck = checkSides();
			if (sidesCheck!=1) sidesClear=0;
			timeIsUp = 0;
			start_t = clock();
		}
	}
	move_forever(0,0);
	if (!frontClear) return 0; // obstacle in front
	return 1; // obstacle to the right/left
}

int checkSides() {
	/*
		return 1 if sides are clear
		return 0 if obstacle left
		return 2 if obstacle right
	*/
	int front;
	move_forever(0,0);
	turn_approx(30); // left ?
	front = isThereSomethingInFront();
	if (front) {
		return 0;
	}
	turn_approx(-60); // right?
	front = isThereSomethingInFront();
	if (front) {
		return 2;
	}
	turn_approx(30); // now facing back in front
	return 1; // sides are clear
}





