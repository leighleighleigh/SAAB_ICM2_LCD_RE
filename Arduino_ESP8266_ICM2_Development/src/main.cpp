#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>
#include <myBitmap.h>

// A few code changes because I was making absolute sure my wiring was the problem
// and not the code.

#define WIDTH 106
#define HEIGHT 64
SAAB_ICM2 display = SAAB_ICM2();

void setup()
{
	Serial.begin(115200);
	Serial.println("ICM2 DISPLAY LIBRARY TEST");
	Wire.begin();

	Wire.setClock(400000);

	if (!display.begin())
	{
		Serial.println(F("ICM2 begin() failed"));
		for (;;)
		{
		}
	}

	// Clean the debris off the display
	display.forceClear();
	display.clearDisplay();
	display.setCursor(1,1);
	display.setTextSize(1);
	display.fillScreen(ICM2_OFF);

	display.fillRect(0,0,WIDTH,9,ICM2_ON);
	display.setTextColor(ICM2_OFF);
	display.println("QUEUE");

	display.setTextColor(ICM2_ON);
	display.setCursor(0,11);
	display.println("SONG 1 ");
	display.println("SONG 2");
	display.println("SONG 3");
	display.println("SONG 4");
	display.println("SONG 5");
	display.println("SONG 6");
	display.println("SONG 7");

	display.display();
	delay(1000);
}

float val = 0;

void loop()
{
	val += 0.01;
	// Clear rect
	int sz = 48;
	int xOffset = 8;
	display.fillRect(WIDTH-sz-xOffset,HEIGHT-sz,sz,sz,ICM2_OFF);
	int YUval = abs(cos(val)*sz);
	display.fillRect(WIDTH-sz-xOffset,HEIGHT-sz,sz,YUval,ICM2_ON);
	delay(10);
	display.display();
	Serial.println(YUval);
}