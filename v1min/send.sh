# usage: ./send.sh file
# action: will send file from current laptop to the robot
# authors: Durville and Filiu
scp $1 robot@ev3dev.local:~/myOS/v1min
