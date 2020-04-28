#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>

#include <Fonts/Picopixel.h>

SAAB_ICM2 display = SAAB_ICM2();

const char* warnText = "WARNING";

void drawFancyString(const char *buf, int x, int y, int padding, int outlineThickness)
{
	int w = 0;
	int h = 0;

	// Calculate text bounds
	int16_t xT, yT;
    uint16_t wT, hT;
	display.getTextBounds(warnText,0,0,&xT,&yT,&wT,&hT);

	w = wT;
	h = hT;

	int xl0 = x - (w/2) - padding - outlineThickness;
	int xl1 = x + (w/2) + padding + outlineThickness;
	int yl0 = y - (h/2) - padding - outlineThickness;
	int yl1 = y + (h/2) + padding + outlineThickness;

	int x0 = x - (w/2) - padding;
	int w0 = w + 2*padding;
	int x1 = x + (w/2) + padding;

	int y0 = y - (h/2) - padding;
	int h0 = h + 2*padding;
	int y1 = y + (h/2) + padding;

	// Outline
	for(int i = xl0; i<xl1; i++)
	{
		for(int j = yl0; j<yl1; j++)
		{
			if( (i+j) % 4 == 0)
			{
				display.drawPixel(i,j,ICM2_ON);
			}
		}
	}	

	// Clear the inside rect
	display.fillRect(x0,y0,w0,h0,ICM2_OFF);

	// Put text
	display.setCursor(x - (w/2),y - (yT/2) - 1);
	display.print(buf);
}

void setup()
{
	Serial.begin(115200);
	Serial.println("ICM2 DISPLAY LIBRARY TEST");
	delay(100);
	
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
	
	display.setCursor(0,0);
	display.setTextSize(3);
	display.setTextWrap(false); // This doesn't work with 'true' for some reason ?
	display.setTextColor(ICM2_ON);
	display.setFont(&Picopixel);

	display.fillScreen(ICM2_OFF);
	
	drawFancyString(warnText,16,16,2,4);

	display.display();
}

double val = 0;

void loop()
{	
	val += 0.2;

	display.fillScreen(ICM2_OFF);
	double i = map(sin(val)*10,-10,10,3,8);
	double j = map(cos(val)*10,-10,10,4,8);

	drawFancyString(warnText,display.width()/2 + random(-1,1),display.height()/2 + random(-1,1),i,j);
	
	display.display();
	Serial.println(sin(val)*10);
	delay(50);
}