#!/bin/sh

echo reset > /sys/class/tacho-motor/motor$1/command
