#!/bin/sh

echo reset > /sys/class/tacho-motor/motor0/command
echo reset > /sys/class/tacho-motor/motor1/command
echo reset > /sys/class/tacho-motor/motor2command

