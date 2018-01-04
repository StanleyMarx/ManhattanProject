# Notes

## Sending the Robot Position
[Position Infos:](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017#position)</br>
* x is the **x coordinate of the robot in multiple of 5cm** (e.g., 2 means 10cm). This field is a **signed 16-bit little-endian integer**.
* y is the **y coordinate of the robot in multiple of 5cm**. This field is a **signed 16-bit little-endian integer**.

## Sending the Map
[Map Info:](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017#mapdata)
* x is the **x coordinate of the pixel in multiple of 5cm**. This field is a **signed 16-bit little-endian integer**.
* y is the **y coordinate of the pixel in multiple of 5cm**. This field is a **signed 16-bit little-endian integer**.
* 'R', 'G', and 'B' are the red, green, blue values of the pixel respectively, valued from 0 to 255. 
