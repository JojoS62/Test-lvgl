// IMPORTANT: LIBRARY MUST BE SPECIFICALLY CONFIGURED FOR EITHER TFT SHIELD
// OR BREAKOUT BOARD USAGE.  SEE RELEVANT COMMENTS IN Adafruit_TFTLCD_16bit_STM32.h

// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

#include "Adafruit_common.h"
#include "ili9341.h"

#ifdef __MBED__
volatile uint16_t *fsmcCommand;
volatile uint16_t *fsmcData;

int fsmc_lcd_init() {

  // enable peripherial clocks
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_FSMC_CLK_ENABLE();

  // set GPIO Alternate Function FSMC
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Alternate = GPIO_AF12_FSMC;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 |
                           GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                           GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                           GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                           GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

  FSMC_NORSRAM_InitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMInitStructure.NSBank = FSMC_NORSRAM_BANK1;                       // ??
  FSMC_NORSRAMInitStructure.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  FSMC_NORSRAMInitStructure.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  FSMC_NORSRAMInitStructure.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  FSMC_NORSRAMInitStructure.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;    // disable
  FSMC_NORSRAMInitStructure.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  FSMC_NORSRAMInitStructure.WrapMode = FSMC_WRAP_MODE_DISABLE;
  FSMC_NORSRAMInitStructure.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  FSMC_NORSRAMInitStructure.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  FSMC_NORSRAMInitStructure.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  FSMC_NORSRAMInitStructure.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  FSMC_NORSRAMInitStructure.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  FSMC_NORSRAMInitStructure.WriteBurst = FSMC_WRITE_BURST_DISABLE;  // enable?
  FSMC_NORSRAMInitStructure.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
  FSMC_NORSRAMInitStructure.WriteFifo = FSMC_WRITE_FIFO_ENABLE;
  FSMC_NORSRAMInitStructure.PageSize = 0;

  HAL_StatusTypeDef status = HAL_OK;
  status = FSMC_NORSRAM_Init(FSMC_Bank1, &FSMC_NORSRAMInitStructure);

  FSMC_NORSRAM_TimingTypeDef FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMTimingInitStructure.AddressSetupTime = 7; // 100 ns (ADDSET+1)*12.5ns = CS to RW
  FSMC_NORSRAMTimingInitStructure.AddressHoldTime = 0;
  FSMC_NORSRAMTimingInitStructure.DataSetupTime = 3;    // 50 ns (DATAST+1)*12.5ns = RW length
  FSMC_NORSRAMTimingInitStructure.BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.CLKDivision = 0;
  FSMC_NORSRAMTimingInitStructure.DataLatency = 0;
  FSMC_NORSRAMTimingInitStructure.AccessMode = FSMC_ACCESS_MODE_A;
  FSMC_NORSRAM_Init(FSMC_Bank1, &FSMC_NORSRAMInitStructure);

  status = FSMC_NORSRAM_Timing_Init(FSMC_Bank1,  &FSMC_NORSRAMTimingInitStructure, FSMC_NORSRAM_BANK1);

  /* Enable FSMC NOR/SRAM Bank1 */
  __FSMC_NORSRAM_ENABLE(FSMC_Bank1, FSMC_NORSRAM_BANK1);

  fsmcCommand    =  (volatile uint16_t*)NOR_MEMORY_ADRESS1;   // clears A18
  fsmcData       =  (fsmcCommand+(1<<18));                    // sets A18

  return status;
};
#endif

#ifndef USE_FSCM
volatile uint32_t *ctrl_port, *data_port;
uint16_t wr_bitmask, rs_bitmask, cs_bitmask;
#endif
/*****************************************************************************/
// Constructor for shield (fixed LCD control lines)
/*****************************************************************************/
#ifndef __MBED__
Adafruit_TFTLCD_16bit_STM32 :: Adafruit_TFTLCD_16bit_STM32(void)
: Adafruit_GFX(TFTWIDTH, TFTHEIGHT)
{
}
#else
Adafruit_TFTLCD_16bit_STM32 :: Adafruit_TFTLCD_16bit_STM32(PinName rst, uint16_t width, uint16_t height) :
    Adafruit_GFX(width, height),
    tft_rst(rst)
{
}
#endif/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::begin(uint16_t id)
{
	reset();

	if (id == 0x9341) {
	    driver = ID_9341;
	    ili9341_begin();
	} else {
	    driver = ID_UNKNOWN;
	}
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::reset(void)
{
#ifdef USE_FSMC
	fsmc_lcd_init();
#else
	ctrl_port = &(TFT_CNTRL_PORT->regs->BSRR);
	data_port = &(TFT_DATA_PORT->regs->ODR);
	wr_bitmask = digitalPinToBitMask(TFT_WR_PIN);
	rs_bitmask = digitalPinToBitMask(TFT_RS_PIN);
	cs_bitmask = digitalPinToBitMask(TFT_CS_PIN);
	//Set control lines as output
	//cntrlRegs->CRL = (cntrlRegs->CRL & 0xFFFF0000) | 0x00003333;
#if 0 // used TFT does not support RD operation
	pinMode(TFT_RD_PIN, OUTPUT);
	RD_IDLE;
#endif
	pinMode(TFT_WR_PIN, OUTPUT);
	pinMode(TFT_RS_PIN, OUTPUT);
	pinMode(TFT_CS_PIN, OUTPUT);
	CS_IDLE; // Set all control bits to HIGH (idle)
	CD_DATA; // Signals are ACTIVE LOW
	WR_IDLE;
/* testing PB4 - sometimes reserved by debug port, see http://www.stm32duino.com/viewtopic.php?f=35&t=1130&p=24289#p24289
	pinMode(PB4, OUTPUT);
	digitalWrite(PB4, HIGH);
	while (1) {
		CS_ACTIVE;
		digitalWrite(PB4, LOW);
		digitalWrite(PB4, HIGH);
		CS_IDLE;
		delay(1000);
	}
*/
	//set up data port to write mode.
	setWriteDir();
#endif // USE_FSCM

#ifndef __MBED__
	// toggle RST low to reset
	if (TFT_RST_PIN >= 0) { // don't use PA0 as reset !!!
		pinMode(TFT_RST_PIN, OUTPUT);
		digitalWrite(TFT_RST_PIN, HIGH);
		delay(100);
		digitalWrite(TFT_RST_PIN, LOW);
		delay(100);
		digitalWrite(TFT_RST_PIN, HIGH);
		delay(100);
	}
#else
  if (tft_rst.is_connected())
  {
      tft_rst = 1;
      wait_ms(100);

      tft_rst = 0;
      wait_ms(100);

      tft_rst = 1;
      wait_ms(100);
  }
#endif
}

/*****************************************************************************/
// Sets the LCD address window (and address counter, on 932X).
// Relevant to rect/screen fills and H/V lines.  Input coordinates are
// assumed pre-sorted (e.g. x2 >= x1).
/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
		ili9341_setAddrWindow(x1, y1, x2, y2);
}

/*****************************************************************************/
// Fast block fill operation for fillScreen, fillRect, H/V line, etc.
// Requires setAddrWindow() has previously been called to set the fill
// bounds.  'len' is inclusive, MUST be >= 1.
/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::flood(uint16_t color, uint32_t len)
{
  CS_ACTIVE_CD_COMMAND;
  if (driver == ID_9341) {
      writeCmd(ILI9341_MEMORYWRITE);
  }

  // Write first pixel normally
  CD_DATA;
#ifndef USE_FSMC
  writeData_(color);
#endif
  uint16_t blocks = len>>3;
  // optimized write in group of 8 consecutive strobes
  while ( (blocks--) ) {
	  WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE;
  }
  // remaining less than 8 bytes
  len &= 0x07;
  while ( (len--) ) {
	  WR_STROBE;
  }
  CS_IDLE;
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t x2;

  // Initial off-screen clipping
  if((length <= 0     ) ||
     (y      <  0     ) || ( y                  >= _height) ||
     (x      >= _width) || ((x2 = (x+length-1)) <  0      )) return;

  if(x < 0) {        // Clip left
    length += x;
    x       = 0;
  }
  if(x2 >= _width) { // Clip right
    x2      = _width - 1;
    length  = x2 - x + 1;
  }

  setAddrWindow(x, y, x2, y);
  flood(color, length);
//  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
//  else                  hx8347g_setLR();
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t y2;

  // Initial off-screen clipping
  if((length <= 0      ) ||
     (x      <  0      ) || ( x                  >= _width) ||
     (y      >= _height) || ((y2 = (y+length-1)) <  0     )) return;
  if(y < 0) {         // Clip top
    length += y;
    y       = 0;
  }
  if(y2 >= _height) { // Clip bottom
    y2      = _height - 1;
    length  = y2 - y + 1;
  }

  setAddrWindow(x, y, x, y2);
  flood(color, length);
//  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
//  else                  hx8347g_setLR();
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::fillRect(int16_t x1, int16_t y1, int16_t w, int16_t h, uint16_t fillcolor)
{
	//Serial.println("\n::fillRect...");
  int16_t  x2, y2;

  // Initial off-screen clipping
  if( (w            <= 0     ) ||  (h             <= 0      ) ||
      (x1           >= _width) ||  (y1            >= _height) ||
     ((x2 = x1+w-1) <  0     ) || ((y2  = y1+h-1) <  0      )) return;
  if(x1 < 0) { // Clip left
    w += x1;
    x1 = 0;
  }
  if(y1 < 0) { // Clip top
    h += y1;
    y1 = 0;
  }
  if(x2 >= _width) { // Clip right
    x2 = _width - 1;
    w  = x2 - x1 + 1;
  }
  if(y2 >= _height) { // Clip bottom
    y2 = _height - 1;
    h  = y2 - y1 + 1;
  }

  setAddrWindow(x1, y1, x2, y2);
  flood(fillcolor, (uint32_t)w * (uint32_t)h);
//  if(driver == ID_932X) setAddrWindow(0, 0, _width - 1, _height - 1);
//  //else                  hx8347g_setLR();
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::fillScreen(uint16_t color)
{
    // For these, there is no settable address pointer, instead the
    // address window must be set for each drawing operation.  However,
    // this display takes rotation into account for the parameters, no
    // need to do extra rotation math here.
  setAddrWindow(0, 0, _width - 1, _height - 1);
  flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    // Clip
    if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

    setAddrWindow(x, y, x+1, y+1);
    writeRegister16(0x2C, color);
}

/*****************************************************************************/
// Draw an image bitmap (16bits per color) at the specified position from the provided buffer.
/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t * bitmap)
{
	if ( x>=0 && (x+w)<_width && y>=0 && (y+h)<=_height ) {
		// all pixel visible, do it in the fast way
		setAddrWindow(x,y,x+w-1,y+h-1);
		pushColors((uint16_t*)bitmap, w*h, true);
	} else {
		// some pixels outside visible area, do it in the classical way to disable off-screen points
		int16_t i, j;
		uint16_t * colorP = (uint16_t*)bitmap;
		for(j=0; j<h; j++) {
			for(i=0; i<w; i++ ) {
				drawPixel(x+i, y+j, *colorP++);
			}
		}
	}
}

/*****************************************************************************/
// Issues 'raw' an array of 16-bit color values to the LCD; used
// externally by BMP examples.  Assumes that setWindowAddr() has
// previously been set to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).
/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::pushColors(uint16_t *data, int16_t len, boolean first)
{
  CS_ACTIVE_CD_COMMAND;
  if(first == true) { // Issue GRAM write command only on first call
       writeCmd(0x2C);
  }
  CD_DATA;
  while(len--) {
    writeData(*data++);
  }
  CS_IDLE;
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::invertDisplay(boolean i)
{
		writeCommand( i ? ILI9341_INVERTON : ILI9341_INVERTOFF);
		CS_IDLE;
}

/*****************************************************************************/
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
// color coding on bits:
// high byte sill be sent first
// bit nr: 		15	14	13	12	11	 10	09	08		07	06	05	 04	03	02	01	00
// color/bit:	R5	R4	R3	R2	R1 | G5	G4	G3		G2	G1	G0 | B5	B4	B3	B2	B1
// 								R0=R5											B0=B5
/*****************************************************************************/
uint16_t Adafruit_TFTLCD_16bit_STM32::color565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	//return ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
}

/*****************************************************************************/
void Adafruit_TFTLCD_16bit_STM32::setRotation(uint8_t x)
{
  // Call parent rotation func first -- sets up rotation flags, etc.
  Adafruit_GFX::setRotation(x);
  // Then perform hardware-specific rotation operations...

   // MEME, HX8357D uses same registers as 9341 but different values
   uint16_t t;

   switch (rotation) {
   case 1:
     t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
     break;
   case 2:
     t = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR;
     break;
   case 3:
     t = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
     break;
   case 0:
   default:
    t = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
    break;
  }
   writeRegister8(ILI9341_MADCTL, t ); // MADCTL
   // For 9341, init default full-screen address window:
   //setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
    setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here
}

/****************************************************************************
uint8_t read8_(void)
{
  RD_ACTIVE;
  delayMicroseconds(10);
  uint8_t temp = ( (dataRegs->IDR>>TFT_DATA_SHIFT) & 0x00FF);
  delayMicroseconds(10);
  RD_IDLE;
  delayMicroseconds(10);
  return temp;
}*/


#if 0 // used TFT cannot be read
/*****************************************************************************/
// Because this function is used infrequently, it configures the ports for
// the read operation, reads the data, then restores the ports to the write
// configuration.  Write operations happen a LOT, so it's advantageous to
// leave the ports in that state as a default.
/*****************************************************************************/
uint16_t Adafruit_TFTLCD_16bit_STM32::readPixel(int16_t x, int16_t y)
{
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return 0;

  if(driver == ID_932X) {

    return ili932x_readPixel(x, y);

  } else if(driver == ID_7575) {

    uint8_t r, g, b;
    writeRegisterPair(HX8347G_COLADDRSTART_HI, HX8347G_COLADDRSTART_LO, x);
    writeRegisterPair(HX8347G_ROWADDRSTART_HI, HX8347G_ROWADDRSTART_LO, y);
    writeCommand(0x22); // Read data from GRAM
    setReadDir();  // Set up LCD data port(s) for READ operations
    CD_DATA;
    read8(r);      // First byte back is a dummy read
    read8(r);
    read8(g);
    read8(b);
    setWriteDir(); // Restore LCD data port(s) to WRITE configuration
    CS_IDLE;
    return (((uint16_t)r & B11111000) << 8) |
           (((uint16_t)g & B11111100) << 3) |
           (           b              >> 3);
  } else return 0;
}

/*****************************************************************************/
uint16_t Adafruit_TFTLCD_16bit_STM32::readID(void)
{
  /*
  for (uint8_t i=0; i<128; i++) {
    Serial.print("$"); Serial.print(i, HEX);
    Serial.print(" = 0x"); Serial.println(readReg(i), HEX);
  }
  */
    /*
      Serial.println("!");
      for (uint8_t i=0; i<4; i++) {
        Serial.print("$"); Serial.print(i, HEX);
        Serial.print(" = 0x"); Serial.println(readReg(i), HEX);
      }
    */
/**/
  if (readReg32(0x04) == 0x8000) { // eh close enough
    // setc!
    writeRegister24(HX8357D_SETC, 0xFF8357);
    delay(300);
    //Serial.println(readReg(0xD0), HEX);
    if (readReg32(0xD0) == 0x990000) {
      return 0x8357;
    }
  }

  uint16_t id = readReg32(0xD3);
  if (id != 0x9341) {
    id = readReg(0);
  }
	//Serial.print("ID: "); Serial.println(id,HEX);
  return id;
}

/*****************************************************************************/
uint32_t readReg32(uint8_t r)
{
  uint32_t id;
  uint8_t x;

  // try reading register #4
  writeCommand(r);
  setReadDir();  // Set up LCD data port(s) for READ operations
  CD_DATA;
  delayMicroseconds(50);
  read8(x);
  id = x;          // Do not merge or otherwise simplify
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id  |= x;        // shenanigans that are going on.
  CS_IDLE;
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration
  return id;
}
/*****************************************************************************/
uint16_t readReg(uint8_t r)
{
  uint16_t id;
  uint8_t x;

  writeCommand(r);
  setReadDir();  // Set up LCD data port(s) for READ operations
  CD_DATA;
  delayMicroseconds(10);
  read8(x);
  id = x;          // Do not merge or otherwise simplify
  id <<= 8;              // these lines.  It's an unfortunate
  read8(x);
  id |= x;        // shenanigans that are going on.
  CS_IDLE;
  setWriteDir();  // Restore LCD data port(s) to WRITE configuration

  //Serial.print("Read $"); Serial.print(r, HEX); 
  //Serial.print(":\t0x"); Serial.println(id, HEX);
  return id;
}
#endif // used TFT cannot be read

/*****************************************************************************/
void writeRegister8(uint16_t a, uint8_t d)
{
  writeCommand(a);
  CD_DATA;
  writeData(d&0x00FF);
  CS_IDLE;
}

/*****************************************************************************/
void writeRegister16(uint16_t a, uint16_t d)
{
  writeCommand(a);
  CD_DATA;
  writeData(d);
  CS_IDLE;
}

/*****************************************************************************/
void writeRegisterPair(uint16_t aH, uint16_t aL, uint16_t d)
{
  writeRegister8(aH, d>>8);
  writeRegister8(aL, d);
}

/****************************************************************************
void writeRegister24(uint16_t r, uint32_t d)
{
  writeCommand(r); // includes CS_ACTIVE
  CD_DATA;
  write8(d >> 16);
  write8(d >> 8);
  write8(d);
  CS_IDLE;
}*/

/*****************************************************************************/
void writeRegister32(uint16_t r, uint32_t d)
{
  writeCommand(r);
  CD_DATA;
  writeData((d >> 24));
  writeData((d >> 16)&0x00FF);
  writeData((d >> 8)&0x00FF);
  writeData(d&0x00FF);
  CS_IDLE;
}
