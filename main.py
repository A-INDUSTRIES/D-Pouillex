import gc
gc.collect()

from adafruit_crickit import crickit
from adafruit_circuitplayground import cp
import adafruit_tcs34725
import board
from time import sleep

class Bot():
    def __init__(self):
        i2c = board.I2C()
        self.sensor = adafruit_tcs34725.TCS34725(i2c)
        self.sensor.integration_time = 2.4
        self.sensor.gain = 60

    def is_white(self, colors):
        if all([color > 70 for color in colors]):
            return True
        return False

    def get_color(self):
        color_rgb = self.sensor.color_raw
        colors = [255/1024 * color for color in color_rgb]
        return colors
    
    def change_led(self, i_list: list, color: list):
        rgb = tuple(color[0:-1])
        for i in i_list:
            crickit.onboard_pixel[i] = rgb
    
    def rotate_servo(self, deg):
        serv = crickit.servo_1
        serv.actuation_range = 180
        serv.angle = 0
        serv.angle = deg

    def rotate_motor(self, thro):
        crickit.dc_motor_1.throttle = thro

if __name__ == "__main__":
    bot = Bot()
    for _ in range(0,20):
        rgbs = bot.get_color()
        bot.change_led([0], rgbs)
    bot.rotate_servo(0)
    sleep(2)
    bot.rotate_servo(180)
    bot.rotate_motor(1)
    sleep(1)
    bot.rotate_motor(0)