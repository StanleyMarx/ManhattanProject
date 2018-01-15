# Tests on Robot - Dec 13

The robot can now move forward for a given amount of time (in seconds), or until it finds an obstacle (using the Sonar).<br/>
It can also use the Gyro to turn right and left, offically following an angle of 90 degrees but we'll have to be more exact on this.<br/>
I did not work on the Pelle Tacho, but we'll have to figure out how to add boundaries to it so it does not try to go out of its bounds.<br/>

Connection thru USB:
```sh
$ nm-connection-editor
$ ssh robot@10.42.0.3
```

## Documentation
[Doc on the tachos by ev3](http://www.ev3dev.org/docs/tutorials/tacho-motors/)

## Makefile
I added this Makefile, which only takes what was in ```compile```.<br/>
We can compile and run the ```MAIN.c``` file by typing in the terminal:</br>
```$ make run``` 
* compiles (only what changed since last compilation)
* runs MAIN

## MAIN.c
```findMotors(sn_left, sn_right, sn_pelle)``` was not assigning the right values to ```sn_left```, ```sn_right``` and ```sn_pelle```.<br/>
So ```forward()``` did not have the right arguments and was not calling the tachos from the right place (don't know why there weren't any error msg though..).<br/>

So I divided the old ```findMotors``` into three subfunctions: ```findLeftMotor```, ```findRightMotor```, ```findPelleMotor```. Each one of them returns the updated value of the corresponding sn value.<br/>

Once I had the right sn values for all three motors, I used these tacho functions:
* ```set_tacho_speed_sp```
* ```set_tacho_command```

But we could also use:
* ```set_tacho_time_sp```
* ```get_tacho_max_speed```
* ```multi_set_tacho_command```
* ```get_tacho_address```

There are a bunch of them and they're all pretty well explained [here](http://in4lio.github.io/ev3dev-c/group__ev3__tacho.html#ga721de9654f98c1151de1734365e7e5ef).<br/>

### New functions
#### Go forward for a certain amount of time
```C
void forwardTimed(uint8_t sn_left, uint8_t sn_right, int seconds)
```
#### Go forward until finds an obstacle
Threshold at 100 looks good
```C
void forwardSonar(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar, float sonarThreshold)
```
#### Turn right and left
Not very exact so we'll have to see to improve them
```C
void turnRight(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro)
void turnLeft(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro)
```


