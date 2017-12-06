# Tests - 05-12-17

Two terminals: 
* one local for my machine
* the robot

Communication between the two with:
```scp ./robotprogs/* robot@ev3dev.local:OS_Robot_Project_Fall2017/nosTest```

## Executing MAIN (after a looot of debugging)
### Test 1 - just checking main(), findSensors(), findMotors()
#### Results
Bunch of warnings at compilation (such and such variable can be used uninitialized in the subfunctions)
```
robot@ev3dev:~/OS_Robot_Project_Fall2017/nosTest$ ./MAIN
Waiting tacho is plugged...
*** ( EV3 ) Hello! ***
LEGO_EV3_M_MOTOR LEFT is found
LEGO_EV3_M_MOTOR RIGHT is found
LEGO_EV3_M_MOTOR PELLE is NOT found
Found sensors:
	type = lego-ev3-touch
	port = in4
	mode = TOUCH
	value0 = 0
	type = ht-nxt-compass
	port = in1:i2c1
	mode = COMPASS
	value0 = 180
	type = lego-ev3-us
	port = in2
	mode = US-DIST-CM
	value0 = 1640
	type = lego-ev3-gyro
	port = in3
	mode = GYRO-ANG
	value0 = 0
COMPASS found, reading compass...
(180.000000)
SONAR found, reading sonar...
(1640.000000)
GYRO found, reading sonar...
(0.000000)
gyro val: 0.000000*** ( EV3 ) Bye! ***
```
#### Analyse
* Pelle not found: looked for wrong port (A instead of D)
* Need to check the different values for the sensors - compass at 180 and gyro at 0 kinda weird

### Test 2 - same as test 1 while moving robot to see if sensors' values change
#### Results
```
robot@ev3dev:~/OS_Robot_Project_Fall2017/nosTest$ ./MAIN
Waiting tacho is plugged...
*** ( EV3 ) Hello! ***
LEGO_EV3_M_MOTOR LEFT is found
LEGO_EV3_M_MOTOR RIGHT is found
LEGO_EV3_M_MOTOR PELLE is found
Found sensors:
  type = lego-ev3-touch
  port = in4
  mode = TOUCH
  value0 = 0
  type = ht-nxt-compass
  port = in1:i2c1
  mode = COMPASS
  value0 = 181
  type = lego-ev3-us
  port = in2
  mode = US-DIST-CM
  value0 = 1646
  type = lego-ev3-gyro
  port = in3
  mode = GYRO-ANG
  value0 = 3
COMPASS found, reading angle...
(181.000000)
SONAR found, reading distance...
(1646.000000)
GYRO found, reading angle...
(3.000000)
gyro val: 0.000000*** ( EV3 ) Bye! ***
```
#### Analyse
* OK pour pelle
* Values for gyro and compass changed a little bit (cuz I actually forgot to move the robot between test 1 and 2) so looks good to go
* last printf of gyro_val = 0 both times and therefore not equal to the other value found right before (3.0 in test 2)
* detail but added a \n to the last printf of gyro_val

### Test 3 - try to implement tests for different sensors - GYRO
So added the .h version for the .c programs that were already working with Alix last week. <br/>
Trying with test-gyro.c<br/>
First time trying to use a function located in .h file.
#### Results
Compilation problems when trying to add test-gyro.h<br/>
We have to include the test-gyro.c file in the compilation process with gcc but didn't manage to do it cuz complicated with all of the ev3 modules already.<br/>
Tried to put the test-gyro.* files in the ev3 directory with all of the other but did not work...
```
MAIN.o: In function `main':
MAIN.c:(.text.startup+0x90): undefined reference to `testGyro'
collect2: error: ld returned 1 exit status
```
#### Analyse
We'll probably have to ask someone how to do this. Either a group that's done it successfully (Italians) or the professor (or PHD student). <br/>
The issue is to add our own .h file to the compilation process that is already pretty complicated with the ev3 modules.
```compile```, and the ```../tests``` directory should not have changed...

### Test 4 - test-gyro in MAIN.c
#### Results
Works!
```
GYRO sensor found, reading angle...
(20.000000)
```
#### Analyse
Now needs to find out how to request this value at one specific moment.

### Test 5 - getGyro in MAIN.c
To request the gyro value at one given moment
#### Results
```
robot@ev3dev:~/OS_Robot_Project_Fall2017/nosTest$ ./MAIN
Waiting tacho is plugged...
*** ( EV3 ) Hello! ***
LEGO_EV3_M_MOTOR LEFT is found
LEGO_EV3_M_MOTOR RIGHT is found
LEGO_EV3_M_MOTOR PELLE is found
GYRO sensor found, reading angle...
(-19.000000)
gyro val: -19.000000
*** ( EV3 ) Bye! ***
```
#### Analyse
Looks good! Gotta try the other sensors now.

### Test 6 - same as test 5 with Sonar and Compass
#### Results
```
robot@ev3dev:~/OS_Robot_Project_Fall2017/nosTest$ ./MAIN
Waiting tacho is plugged...
*** ( EV3 ) Hello! ***
LEGO_EV3_M_MOTOR LEFT is found
LEGO_EV3_M_MOTOR RIGHT is found
LEGO_EV3_M_MOTOR PELLE is found
COMPASS found, reading angle...
(169.000000)
SONAR found, reading distance...
(1649.000000)
GYRO found, reading angle...
(-19.000000)
[DEBUG] reading GYRO
[DEBUG] GYRO: -19.000000
GYRO val: -19.000000
[DEBUG] reading COMPASS
[DEBUG] COMPASS: 169.000000
COMPASS val: 169.000000
[DEBUG] reading SONAR
[DEBUG] SONAR: 1654.000000
SONAR val: 1654.000000
*** ( EV3 ) Bye! ***
```
#### Analyse
Good to go!

### Test 7 - turn right using gyro
#### Results
Nope, problem with the different motors.. utils/deux-moteurs.c works well for both motors but when doing the same thing in MAIN.c, there is only sn_pelle moving. Probably a mess-up when assigning ports etc...
#### Analyse


