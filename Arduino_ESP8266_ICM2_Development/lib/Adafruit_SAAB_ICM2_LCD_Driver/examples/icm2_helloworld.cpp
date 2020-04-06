#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>

SAAB_ICM2 display = SAAB_ICM2(); // Bastardised usage of Adafruit_GFX. No SPI option. No other parameters. Sorry Adafruit!

void setup()
{
	Serial.begin(115200);
	Serial.println("ICM2 DISPLAY LIBRARY TEST");

	// Wire must be initialised before display.begin.
	Wire.begin();
	Wire.setClock(400000);

	// Begin the display :)
	if (!display.begin())
	{
		Serial.println(F("ICM2 begin() failed"));
		for (;;)
		{
		}
	}

	// Clean the debris off the display, 
    // this is because the usual clearDisplay does not address the very top row at the moment,
    // and in my prototype the LCD is improperly grounded, so there are artifacts on startup.
	// This WRITES to the display, UNLIKE display.clearDisplay().
	display.forceClear();

	// Show hello world text.
	display.clearDisplay();
    display.setCursor(8,8);
    display.setTextSize(1);
    display.setTextColor(ICM2_ON);
    display.println("HELLO, WORLD!");
	display.display();
}


void loop()
{
}