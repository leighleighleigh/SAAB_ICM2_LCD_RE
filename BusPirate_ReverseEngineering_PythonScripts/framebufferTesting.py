#!/usr/bin/python3
# SLOW, USE FLASHROM INSTEAD
import serial
import time
import adafruit_framebuf  # sudo pip3 install adafruit-circuitpython-framebuf
import math

BUSPIRATE_PORT = '/dev/ttyUSB1'  # customize this! Find it in device manager.

# DISPLAY PARAMETERS
# Visible width: 106
# Visible height: 8x8

# Framebuffer
WIDTH = 106
HEIGHT = 64

buffer = bytearray(round(WIDTH * HEIGHT / 8))
fb = adafruit_framebuf.FrameBuffer(
    buffer, WIDTH, HEIGHT, buf_format=adafruit_framebuf.MVLSB)


def send(ser, cmd, storeResult=False):
    """send the command and listen to the response."""
    ser.write(str(cmd+'\n').encode('ascii'))  # send our command

    for line in ser.readlines():  # while there's a response
        print(line.decode('utf-8').strip())  # show it


def lcd_init():
    txtdat = ""
    txtdat += "[0x78,0x00,0x01,0x10]"
    txtdat += "[0x78,0x00,0x01,0x0e,0x12]"
    txtdat += "[0x78,0x00,0x01,0x0e,0x06]"
    txtdat += "[0x78,0x00,0x01,0x0e,0x0a]"

    txtdat += "[0x78,0x00,0x01,0x0e,0x24]"
    txtdat += "[0x78,0x00,0x01,0x0e,0x84]"
    txtdat += "[0x78,0x00,0x01,0x0d,0x0a]"
    txtdat += "[0x78,0x00,0x01,0x0d,0x13]"

    txtdat += "[0x78,0x00,0x01,0x0d,0xb7]"
    txtdat += "[0x78,0x00,0x01,0x0d,0x07]"
    txtdat += "[0x78,0x00,0x01,0x0b,0x04]"
    txtdat += "[0x78,0x00,0x01,0x0b,0x40]"

    send(ser, txtdat)

# Ascii printer for very small framebufs!


def print_buffer(the_fb):
    print("." * (the_fb.width + 2))
    for y in range(the_fb.height):
        print(".", end="")
        for x in range(the_fb.width):
            if fb.pixel(x, y):
                print("*", end="")
            else:
                print(" ", end="")
        print(".")
    print("." * (the_fb.width + 2))


def drawFrameBuf(the_fb):
    for line in range(8):
        # Calculate line cmd
        lineCmd = 0x40 + line

        # Set the line to draw on
        send(ser, "[0x78,0x00,0x01," + hex(lineCmd) +
             "][0x78,0x00,0x01,0x20][0x78,0x00,0x01,0x8d]")

        # Setup y offset for navigating the fb
        yOffset = line * 8

        # Send data in four chunks
        # Setup data header
        txtdat = "[0x78,0x40,"
        # Populate data
        for x in range(the_fb.width//4):
            charDat = 0b00000000
            for y in range(8):
                if the_fb.pixel(x, y + yOffset):
                    charDat = charDat | (0b10000000 >> y)
            # Append charDat to the txtDat as a hex byte
            txtdat += hex(charDat) + ","
        # End the data
        txtdat = txtdat[0:-1]
        txtdat += "]"
        # Send the data
        send(ser, txtdat)

        # Chunk 2
        # Setup data header
        txtdat = "[0x78,0x40,"
        # Populate data
        for x in range(the_fb.width//4, the_fb.width//2):
            charDat = 0b00000000
            for y in range(8):
                if the_fb.pixel(x, y + yOffset):
                    charDat = charDat | (0b10000000 >> y)
            # Append charDat to the txtDat as a hex byte
            txtdat += hex(charDat) + ","
        # End the data
        txtdat = txtdat[0:-1]
        txtdat += "]"
        # Send the data
        send(ser, txtdat)

        # Chunk 3
        txtdat = "[0x78,0x40,"
        # Populate data
        for x in range(the_fb.width//2, the_fb.width//2 + the_fb.width//4):
            charDat = 0b00000000
            for y in range(8):
                if the_fb.pixel(x, y + yOffset):
                    charDat = charDat | (0b10000000 >> y)
            # Append charDat to the txtDat as a hex byte
            txtdat += hex(charDat) + ","
        # End the data
        txtdat = txtdat[0:-1]
        txtdat += "]"
        # Send the data
        send(ser, txtdat)

        # Chunk 4
        # Setup data header
        txtdat = "[0x78,0x40,"
        # Populate data
        for x in range(the_fb.width//2 + the_fb.width//4, the_fb.width):
            charDat = 0b00000000
            for y in range(8):
                if the_fb.pixel(x, y + yOffset):
                    charDat = charDat | (0b10000000 >> y)
            # Append charDat to the txtDat as a hex byte
            txtdat += hex(charDat) + ","
        # End the data
        txtdat = txtdat[0:-1]
        txtdat += "]"
        # Send the data
        send(ser, txtdat)


def text_wrap(str, x, y, color, w, h, border=None):
    # optional box border
    if border is not None:
        fb.rect(x, y, w, h, border)

    cols = w // 6

    # for each row
    j = 0
    for i in range(0, len(str), cols):
        # draw as many chars fit on the line
        fb.text(str[i:i+cols], x, y + j, color)
        j += 8
        # dont overflow text outside the box
        if j >= h:
            break

ser = serial.Serial(BUSPIRATE_PORT, 115200, timeout=1)  # is com free?
assert ser.isOpen()  # throw an exception if we aren't connected
# send(ser, '#')  # reset bus pirate (slow, maybe not needed)
send(ser, 'm')  # change mode (goal is to get away from HiZ)
send(ser, '4')  # i2c mode
send(ser, '4')  # 400kHz
send(ser, 'W')  # turn power supply to ON. Lowercase w for OFF.

lcd_init()

fb.fill(0)
# fb.text("SAAB ICM 2 \nDISPLAY...HACKED!",4,4,True)
# fb.rect(0, 0, fb.width, fb.height, True)
# # fb.line(1, 1, fb.width - 2, fb.height - 2, True)

# for i in range(106):
#     fb.pixel(i,int(42 - 16 * math.sin((i/106) * 4*3.1415)),True)

# for i in range(106):
#     fb.pixel(i,int(28 - 8 * math.sin((i/106) * 4*3.1415)),True)

# fb.text("FROGGY\nCHAIR???",16,16,True)
text_wrap("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",3,3,True,fb.width-6,fb.height-6,False)

drawFrameBuf(fb)

time.sleep(2)

ser.close()
