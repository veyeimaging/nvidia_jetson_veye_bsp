#!/usr/bin/python3

import RPi.GPIO as GPIO

import time

output_pin = 40  # BOARD pin 40

GPIO.setmode(GPIO.BOARD)

GPIO.setup(output_pin,GPIO.OUT)

#while True:
for num in range(0,5):

    GPIO.output(output_pin,GPIO.HIGH)

    time.sleep(0.01)

    GPIO.output(output_pin,GPIO.LOW)

    time.sleep(1)
    
    print (num)


GPIO.cleanup()