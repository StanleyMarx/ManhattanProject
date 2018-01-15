# Main Scripts

Our main important script files are:
* ```main.c```, which is our main script in charge of launching everything.
* ```main-prog.h```, which is where we set up our different test scripts.
* ```myev3.h```, which is where we store all of our functions. 
* ```main2.c``` is a copy of ```main.c```, without the connection to the server (to be used on machine which cannot have the server).

## C Scripts
Then, the ```./mapData``` is a test folder where we present the scripts used to print the map at the end of the game.<br/>
The ```pos.txt``` is where the robot stores its coordinates as it evolves through the map. <br/>
The ```build_map.c``` is the updated version of the script in ```./mapData```, which can be used for debugging purposes on the robot (if a script exits without printing the map, we can always use this ```build_map.c``` script to get the map out of the ```pos.txt``` file.

## Shell Scripts
* ```compile.sh``` is the script we used to compile our ```main.c``` and ```main2.c``` scripts.
* ```send.sh``` was used to send directly files from our host machines to the robot.
