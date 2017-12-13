# Tests on Robot - Dec 13

Main goal is to have the robot go forward, both motors running together at the same time...<br/>

Connection thru USB:
```sh
$ nm-connection-editor
$ ssh robot@10.42.0.3
```

## Documentation
[Doc on the tachos by ev3](http://www.ev3dev.org/docs/tutorials/tacho-motors/)

## Makefile
```$ make run``` 
* compiles (only what changed since last compilation)
* runs MAIN

## MAIN.c
```findMotors(sn_left, sn_right, sn_pelle)``` was not assigning the right values to sn_left, sn_right and sn_pelle.<br/>
So forward() did not have the right arguments and was not calling the tachos at the right place (don't know where there weren't any error msg though..)
