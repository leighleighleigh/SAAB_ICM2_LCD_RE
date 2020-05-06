#include <Arduino.h>
#include <math.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>

#include <Fonts/Picopixel.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

SAAB_ICM2 display = SAAB_ICM2();

void drawBoxStringCentered(const char *buf, int x, int y, int padding, int outlineThickness)
{
	int w = 0;
	int h = 0;

	// Calculate text bounds
	int16_t xT, yT;
    uint16_t wT, hT;
	display.getTextBounds(buf,0,0,&xT,&yT,&wT,&hT);

	w = wT;
	h = hT;

	int xl0 = x - (w/2) - padding - outlineThickness;
	int xl1 = x + (w/2) + padding + outlineThickness + 1;
	int yl0 = y - (h/2) - padding - outlineThickness;
	int yl1 = y + (h/2) + padding + outlineThickness + 1;

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
			display.drawPixel(i,j,ICM2_ON);
		}
	}	

	// Clear the inside rect
	display.fillRect(x0,y0,w0,h0,ICM2_OFF);

	// Put text
	display.setCursor(x - (w/2),y - (yT/2));
	display.print(buf);
}

void fillTexture(int xOffset, int yOffset)
{
	// Fill with a texture
	for(int y = 0; y<display.height(); y++)
	{
		for(int x = 0; x<display.width(); x++)
		{
			// This genereates the texture
			if((x + xOffset + y + yOffset) % 4 == 0)
			{
				display.drawPixel(x,y,ICM2_ON);
			}else{
				display.drawPixel(x,y,ICM2_OFF);
			}
		}
	}	
}

void drawBoxString(const char *buf, int x, int y, int padding, int outlineThickness, uint16_t *cX, uint16_t *cY)
{
	int w = 0;
	int h = 0;

	// Calculate text bounds
	int16_t xT, yT;
    uint16_t wT, hT;
	display.getTextBounds(buf,0,0,&xT,&yT,&wT,&hT);

	w = wT;
	h = hT;

	// Draws the 'background'
	int xl0 = x;
	int xl1 = x + w + 2*padding + 2*outlineThickness;
	int yl0 = y;
	int yl1 = y + h + 2*padding + 2*outlineThickness;

	// Outline
	for(int i = xl0; i<xl1; i++)
	{
		for(int j = yl0; j<yl1; j++)
		{
			display.drawPixel(i,j,ICM2_ON);
		}
	}

	// Used to draw the clear center
	int x0 = x + outlineThickness;
	int w0 = w + 2*padding;
	int y0 = y + outlineThickness;
	int h0 = h + 2*padding;
	// Clear the inside rect
	display.fillRect(x0,y0,w0,h0,ICM2_OFF);

	*cX = x + w + 2*padding + 2*outlineThickness;
	*cY = y + h + 2*padding + 2*outlineThickness;

	// Put text
	display.setCursor(x + outlineThickness + padding,y + outlineThickness + padding - yT);
	display.print(buf);
}

void setup()
{
	Serial.begin(115200);
	Serial.println("ICM2 DISPLAY LIBRARY TEST");
	delay(100);
	
	Wire.begin();
	Wire.setClock(2000000);


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
	
	display.setTextSize(1);
	display.setTextWrap(false); // This doesn't work with 'true' for some reason ?
	display.setTextColor(ICM2_ON);
	
	display.setFont(&Picopixel);
	// display.setFont(&FreeMonoBold9pt7b);
}

// Translate polar to cartesian
void toCartesian2D(double r, double theta, double *x, double *y)
{
	*y = sin(theta) * r;
	*x = cos(theta) * r;
}

void toPolar2D(double x, double y, double *r, double *theta)
{
	*r = sqrt(x*x + y*y);
	*theta = atan2(y,x);
}

void drawAxis()
{
	// display.drawPixel(display.width()/2,display.height()/2,ICM2_ON);
	display.drawFastHLine(0,display.height()/2,display.width(),ICM2_ON);
	display.drawFastVLine(display.width()/2,0,display.height(),ICM2_ON);
}

double test(double x)
{
	return 10*exp(-x/20) * sin(x/5);
	// return sin(x/4)*20;
}

// Plots mathfunc across x and y pixels
void plot(double (*mathfunc)(double),double scale)
{
	double t = 1 / scale;

	double lastx = NULL;
	double lasty = NULL;

	for(double x = -display.width()/2; x<display.width()/2; x+=t)
	{
		double yy = display.height()/2 - mathfunc((double)x);
		double xx = map(x,-display.width()/2,display.width()/2,0,display.width());

		if(x == -display.width()/2)
		{
			display.drawPixel(xx,yy,ICM2_ON);
		}else{
			display.drawLine(lastx,lasty,xx,yy,ICM2_ON);
		}
		lastx = xx;
		lasty = yy;
	}
}

int xVal = 0;

void loop()
{	
	xVal ++;
	fillTexture(xVal,0);
	drawBoxStringCentered("BUILT FROM JETS",display.width() / 2, display.height() / 2,3,1);	
	display.display();
	delay(50);
}