# usage: ./send.sh file
# action: will send file from current laptop to the robot
# authors: Durville and Filiu
scp $1 robot@10.42.0.3:~/myOS/v1
