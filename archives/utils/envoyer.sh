 #usage: ./envoyer.sh file
# action: will send file from Henri's laptop to the robot
scp $1 robot@169.254.100.238:~/OS_Robot_Project_Fall2017/nosTest
