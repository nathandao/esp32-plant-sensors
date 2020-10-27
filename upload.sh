#! /usr/bin/env bash

DEV_PATH=$1
BOARD=esp32:esp32:esp32thing
SKETCH_DIR=plant_sensors

sudo chmod a+rw $DEV_PATH

arduino-cli compile --fqbn $BOARD $SKETCH_DIR && arduino-cli upload --fqbn $BOARD $SKETCH_DIR --port $DEV_PATH
