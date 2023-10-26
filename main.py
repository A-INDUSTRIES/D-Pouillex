"""This code is meant to be copied over to code.py on the controller"""
from adafruit_circuitplayground import cp
import board
import adafruit_tcs34725
import math

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
            cp.pixels[i] = rgb

if __name__ == "__main__":
    bot = Bot()
    for _ in range(0,200):
        rgbs = bot.get_color()
        print(rgbs)
        print(bot.is_white(rgbs))
        bot.change_led([0], rgbs)