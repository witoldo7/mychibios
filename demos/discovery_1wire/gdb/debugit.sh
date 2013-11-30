#!/bin/bash
killall st-util arm-none-eabi-gdb
sleep 1
st-util -v 0 > /dev/null 2>&1 &
sleep 1
arm-none-eabi-gdb ../build/ch.elf
killall st-util
