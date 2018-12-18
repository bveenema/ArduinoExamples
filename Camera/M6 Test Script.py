# QRCode Example
#
# This example shows the power of the OpenMV Cam to detect QR Codes
# without needing lens correction.

import sensor, image, time, pyb

green_led = pyb.LED(2)
blue_led = pyb.LED(3)
blue_led.off()
green_led.off()

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.VGA)
sensor.set_windowing((240, 240)) # look at center 240x240 pixels of the VGA resolution.
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must turn this off to prevent image washout...

clock = time.clock()

startTime = 0
greenOnTime = 0
blueBlinkTime = 0

green_led_is_on = False

while(True):
    clock.tick()
    img = sensor.snapshot()
    codes = img.find_qrcodes()
    if(codes):
        green_led.on()
        blue_led.off()
        green_led_is_on = True
        greenOnTime = pyb.millis()
    else:
        if pyb.elapsed_millis(greenOnTime) > 1000:
            green_led_is_on = False
            green_led.off()
        print('NO CODES')
    for code in img.find_qrcodes():
        img.draw_rectangle(code.rect(), color = 127)
        print(code)
    if(pyb.elapsed_millis(startTime) > 2000):
        sensor.reset()
        sensor.set_pixformat(sensor.GRAYSCALE)
        sensor.set_framesize(sensor.VGA)
        sensor.set_windowing((240, 240)) # look at center 240x240 pixels of the VGA resolution.
        sensor.skip_frames(time = 500)
        sensor.set_auto_gain(False)
        startTime = pyb.millis()

    # Blink the Blue LED
    if pyb.elapsed_millis(blueBlinkTime) > 500:
        if green_led_is_on:
            blue_led.off()
        else:
            blue_led.toggle()
        blueBlinkTime = pyb.millis()

