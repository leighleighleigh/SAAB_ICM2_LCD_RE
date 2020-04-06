#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>
#include <myBitmap.h>

#define WIDTH 106
#define HEIGHT 64
SAAB_ICM2 display = SAAB_ICM2();

void backlightCtrl(bool state, double fadeTimeMs)
{
	// PWM BACKLIGHT
	if(state){
		// increase the LED brightness
		for (int dutyCycle = 0; dutyCycle < 1023; dutyCycle++)
		{
			// changing the LED brightness with PWM
			analogWrite(D3, dutyCycle);
			delay(ceil(fadeTimeMs / 1024));
		}
	}else{
		// decrease the LED brightness
		for (int dutyCycle = 1023; dutyCycle > 0; dutyCycle--)
		{
			// changing the LED brightness with PWM
			analogWrite(D3, dutyCycle);
			delay(ceil(fadeTimeMs / 1024));
		}
	}
}

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
	backlightCtrl(false,0);
	delay(1000);
}

void loop()
{
	display.clearDisplay();
	display.drawBitmap(0, 0, myBitmap, 106, 64, ICM2_ON);
	display.display();
	backlightCtrl(true,2048);
	backlightCtrl(false,2048);
	display.clearDisplay();
	display.display();
	delay(1000);
}