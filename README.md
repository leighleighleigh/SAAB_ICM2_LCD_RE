# SAAB_ICM2_LCD_RE
Collection of schematics and resources I've collected while reverse-engineering the LCD used in SAAB 9-3 "Infotainment Control Module 2".

## Directory Descriptions
- Arduino_ESP8266_ICM2_Development
    - This is the PIO environment where I developed the Arduino_SAAB_ICM2_LCD_Driver, for this display.
    - This driver is referenced as a git submodule.
- BusPirate_ReverseEngineering_PythonScripts
    - While I was first reverse-engineering the I2C commands for the display, I wrote a couple terrible-named scripts to drive the BusPirate v3 I was using.
    - I then chose to move to a C-library, as the BusPirate is incredibly slow!
    - But we have to start somewhere, so I'm keeping this here to "show the process".
- ICM2_PCB_PICS
    - Here you will find actual pictures of the ICM2 PCB, and some have been creatively overlaid in photoshop to allow me to visually trace the connections.
    - This helped me develop a wiring schematic for the display...
- KICAD_SAAB_ICM2_Display
    - Here is the aforementioned schematic, a PDF printout has been supplied for convenience.
    - There are also alot of other notes there pertaining to interfacing with the module.
- KINGSTVIS_DLA_LOGS
    - Here are some KingstVIS digital logic analyser files, which I initially used to sniff the I2C lines (before I even KNEW they were I2C lines.).
    - I then used this info to take it over to the BusPirate and begin reverse-engineering.
- TEST_GRAPHICS
    - Some bitmaps/PSD files I used/made to test the display. 
    - There is a SAAB one, which I thought looked cool.
    - There is also one called "Froggy Chair", which is a reference to the video game series "Animal Crossing".
