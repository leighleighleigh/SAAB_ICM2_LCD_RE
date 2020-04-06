### Hello there
Thanks for checking out my ICM2 display library, it's actually my very first "proper library that I've put effort into", heres a list of mistakes I am aware of...

- Addressing of the display is not fully understood / explained / explored / I am lazy.
    - This means the very top row of the display is not used at the moment.
    - Also it means you are not allowed to use the extra space on the display as a framebuffer, sorry.
    - Also, no scrolling. Sorry again!

- drawPixel routine puts the bytes in "upside down", so in the display() routine, they have to be manually reversed each time. This is done by the reverseB function.

- Uses WIDTH, HEIGHT global variables. This probably isn't good/proper.

- No limiting of max bytes send over I2C, IE no byteSent > WIRE_MAX checks. 
- No option to ask the display driver to initialise I2C (ie: call Wire.begin). Other Adafruit-based drivers have this option, I chose to remove it for simplicity.

- No SPI. This display doesn't support SPI, so... 