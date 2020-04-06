#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#define pgm_read_byte(addr) \
    (*(const unsigned char *)(addr)) ///< PROGMEM workaround for non-AVR
#endif

#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>

// SOME DEFINES AND STATIC VARIABLES USED INTERNALLY -----------------------
#define icm2_swap(a, b) \
    (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

/*!
    @brief  Constructor for the I2C-interfaced ICM2 display.
    @param  w
            Display width in pixels (106)
    @param  h
            Display height in pixels (64)
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
*/
SAAB_ICM2::SAAB_ICM2() : Adafruit_GFX(WIDTH, HEIGHT), buffer(NULL) {}

/*!
    @brief  Destructor for Adafruit_SSD1306 object.
*/
SAAB_ICM2::~SAAB_ICM2(void)
{
    if (buffer)
    {
        free(buffer);
        buffer = NULL;
    }
}

// INTERNAL FUNCTIONS

void SAAB_ICM2::icm2_command1(uint8_t c)
{
    // I2C
    Wire.beginTransmission(i2caddr);
    Wire.write(c);
    Wire.endTransmission();
}

void SAAB_ICM2::icm2_commandList(const uint8_t *c, uint8_t n)
{

    Wire.beginTransmission(i2caddr);
    Wire.write(c, n);
    Wire.endTransmission();
}

// A public version of icm2_command1(), for existing user code that
// might rely on that function. This encapsulates the command transfer
// in a transaction start/end, similar to old library's handling of it.
/*!
    @brief  Issue a single low-level command directly to the ICM2
            display, bypassing the library.
    @param  c
            Command to issue (0x00 to 0xFF, see datasheet).
    @return None (void).
*/
void SAAB_ICM2::icm2_command(uint8_t c)
{
    icm2_command1(c);
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  addr
            I2C address of corresponding ICM2 display.
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool SAAB_ICM2::begin(void)
{
    // Allocate dat memory
    if ((!buffer) && !(buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8))))
        return false;

    // Zero the framebuffer
    clearDisplay();

    // run all the init commands
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x10}, 3);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x12}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x06}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x0a}, 4);

    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x24}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x84}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0x0a}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0x13}, 4);

    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0xb7}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0x07}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0b, 0x04}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0b, 0x40}, 4);

    return true; // Success
}

// DRAWING FUNCTIONS

/*!
    @brief  Set/clear/invert a single pixel. This is also invoked by the
            Adafruit_GFX library in generating many higher-level graphics
            primitives.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  color
            Pixel color, one of: ICM2_ON, ICM2_OFF, ICM2_INVERSE
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void SAAB_ICM2::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x >= 0) && (x < width()) && (y >= 0) && (y < height()))
    {
        switch (color)
        {
        case ICM2_ON:
            buffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
            break;
        case ICM2_OFF:
            buffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
            break;
        case ICM2_INVERSE:
            buffer[x + (y / 8) * WIDTH] ^= (1 << (y & 7));
            break;
        }
    }
}

/*!
    @brief  Get base address of display buffer for direct reading or writing.
    @return Pointer to an unsigned 8-bit array, column-major, columns padded
            to full byte boundary if needed.
*/
uint8_t *SAAB_ICM2::getBuffer(void)
{
    return buffer;
}

/*!
    @brief  Clear contents of display buffer (set all pixels to off).
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void SAAB_ICM2::clearDisplay(void)
{
    memset(buffer, 0, WIDTH * ((HEIGHT + 7) / 8));
}

// REFRESH DISPLAY ---------------------------------------------------------

uint8_t reverseB(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

// Write all zeros after an init to clear debris lmao
void SAAB_ICM2::forceClear(void)
{   
    // Note an extra line is being cleared
    for (int line = 0; line < 9; line++)
    {
        // Set address we are writing too, which is split into "lines" of 8 pixels height.
        uint8_t lineAddr = 0x40 + line;
        uint8_t dat[3] = {0x00, 0x01, lineAddr};
        icm2_commandList(dat, sizeof(dat));
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x20}, 3);
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x8d}, 3);

        Wire.beginTransmission(i2caddr);
        Wire.write((uint8_t)0x40);
        for(int i = 0; i<106; i++)
        {
            Wire.write(0);
        }
        Wire.endTransmission();
    }
}

/*!
    @brief  Push data currently in RAM to SSD1306 display.
    @return None (void).
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void SAAB_ICM2::display(void)
{
    // So basically we go through the ram and smash it out!
    uint8_t *ptr = buffer;
    // First init to the top right corner of the screen, and after WIDTH is done, we increment line address
    for (int line = 0; line < 8; line++)
    {
        // Set address we are writing too, which is split into "lines" of 8 pixels height.
        uint8_t lineAddr = 0x40 + line;
        uint8_t dat[3] = {0x00, 0x01, lineAddr};
        icm2_commandList(dat, sizeof(dat));
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x20}, 3);
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x8d}, 3);

        // Count of pixels or whatever
        uint16_t count = (WIDTH * ((HEIGHT + 7) / 8)) / 8;
        // I2C
        Wire.beginTransmission(i2caddr);
        Wire.write((uint8_t)0x40);
        uint8_t bytesOut = 1;
        while (count--)
        {
            Wire.write(reverseB(*ptr++));
            bytesOut++;
        }
        Wire.endTransmission();
    }
}

