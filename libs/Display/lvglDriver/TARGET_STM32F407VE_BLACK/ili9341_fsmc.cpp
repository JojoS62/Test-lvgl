#include "ili9341_fsmc.h"

extern volatile uint16_t *ili9341_fsmcCommand;
extern volatile uint16_t *ili9341_fsmcData;

#define lcdCommand (*ili9341_fsmcCommand)
#define lcdData    (*ili9341_fsmcData)

#define writeCmd(d)  { lcdCommand = d; }
#define writeData(d) { lcdData = d; }

void ili9341_fsmc_writeCmd(uint16_t cmd) { writeCmd(cmd); };
void ili9341_fsmc_writeData(uint16_t data) { writeData(data); };

static void writeRegister8(uint16_t a, uint8_t d)
{
  writeCmd(a);
  writeData(d&0x00FF);
}

static void writeRegister32(uint16_t r, uint32_t d)
{
  writeCmd(r);
  writeData((d >> 24));
  writeData((d >> 16)&0x00FF);
  writeData((d >> 8)&0x00FF);
  writeData(d&0x00FF);
}

#define ILI9486 1

/**/
#define TFTLCD_DELAY8 0xFF

static const uint8_t ILI9341_regValues_ada[] = {        // Adafruit_TFTLCD only works with EXTC=0
    //                     0xF6, 3, 0x00, 0x01, 0x00,  //Interface Control needs EXTC=1 TM=0, RIM=0
    //            0xF6, 3, 0x01, 0x01, 0x03,  //Interface Control needs EXTC=1 RM=1, RIM=1
    /**/
    //0xF6, 3, 0x09, 0x01, 0x03,  //Interface Control needs EXTC=1 RM=0, RIM=1
    0xB0, 1, 0x40,      //RGB Signal [40] RCM=2
    0xB4, 1, 0x00,      //Inversion Control [02] .kbv NLA=1, NLB=1, NLC=1
    0xC0, 1, 0x23,      //Power Control 1 [26]
    0xC1, 1, 0x10,      //Power Control 2 [00]
    0xC5, 2, 0x2B, 0x2B,        //VCOM 1 [31 3C]
    0xC7, 1, 0xC0,      //VCOM 2 [C0]
    0x36, 1, 0x48,      //Memory Access [00]
    0xB1, 2, 0x00, 0x1B,        //Frame Control [00 1B]
    0xB7, 1, 0x07,      //Entry Mode [00]
//  TFTLCD_DELAY8, 1,
};

/*
.write16: 36_write8: 0__write8: 36__write8: 48_
.write16: 33_write8: 0__write8: 33__write8: 0__write8: 0__write8: 1__write8: 40__write8: 0__write8: 0_
.write16: 37_write8: 0__write8: 37__write8: 0__write8: 0_
.write16: 13_write8: 0__write8: 13_
.write16: 21_write8: 0__write8: 21_
*/
static const uint8_t ILI9341_regValues_post[] = {
    // post-init settings, sniffed from David's lib
    0x36, 1, 0x48,      //Memory Access [00]
    0x33, 6, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00,
    0x37, 2, 0x00, 0x00,
    0x13, 0,            // normaldisp
    0x21, 0,            // invert off
};

/*****************************************************************************/
static void WriteCmdParamN(uint16_t cmd, int8_t N, const uint8_t *block)
{
    writeCmd(cmd);
    while (N-- > 0) {
        uint8_t u8 = *block++;
        writeData(0x00FF & u8);
    }
}

/*****************************************************************************/
static void init_table(const uint8_t *table, int16_t size)
{
    while (size > 0) {
        uint8_t cmd = *table++;
        uint8_t len = *table++;
        if (cmd == TFTLCD_DELAY8) {
            ThisThread::sleep_for(len);
            len = 0;
        } else {
            WriteCmdParamN(cmd, len, table);
            table += len;
        }
        size -= len + 2;
    }
}

static const uint8_t reset_off[] = {
    0x01, 0,                //Soft Reset
    TFTLCD_DELAY8, 150,     // .kbv will power up with ONLY reset, sleep out, display on
    0x28, 0,                //Display Off
    0x3A, 1, 0x55          //Pixel read=565, write=565.
    // Power control
//  0xc0, 2, 0x00, 0x26,
//  0xc1, 2, 0x00, 0x11,
//  0xc5, 4, 0x00, 0x5c, 0x00, 0x4c,
//  0xc7, 2, 0x00, 0x94,
    //
    // frame rate
//  0xb1, 4, 0x00, 0x00, 0x00, 0x1b
#ifndef ILI9486
    // PGAMCTRL(Positive Gamma Control)
    0xE0, 15, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98,
    0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00,
    // NGAMCTRL(Negative Gamma Control)
    0xE1, 15, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
    0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
    // Digital Gamma Control 1
    0xE2, 15, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
    0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
#endif
};

static const uint8_t wake_on[] = {
    0x11, 0,            //Sleep Out
    TFTLCD_DELAY8, 150,
    0x29, 0,            //Display On
    //additional settings
//  0x21, 0,            // invert off
    0x36, 1, 0x48,      //Memory Access
    0xB0, 1, 0x40,      //RGB Signal [40] RCM=2
};


/*****************************************************************************/
void ili9341_fsmc_init()
{
    init_table(reset_off, sizeof(reset_off));
    //init_table(ILI9341_regValues_ada, sizeof(ILI9341_regValues_ada));   //can change PIXFMT
    init_table(wake_on, sizeof(wake_on));
    //init_table(ILI9341_regValues_post, sizeof(ILI9341_regValues_post));
}

/*****************************************************************************/
// Sets the LCD address window. Relevant to all write routines.
// Input coordinates are assumed pre-sorted (e.g. x2 >= x1).
/*****************************************************************************/
void ili9341_fsmc_setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    writeRegister32(ILI9341_COLADDRSET, ((uint32_t)(x1 << 16) | x2)); // HX8357D uses same registers!
    writeRegister32(ILI9341_PAGEADDRSET, ((uint32_t)(y1 << 16) | y2)); // HX8357D uses same registers!
}

/*****************************************************************************/
void ili9341_fsmc_setRotation(uint8_t x)
{
    uint16_t t;

    switch (x) {
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
    writeRegister8(ILI9341_MADCTL, t);  // MADCTL
    // For 9341, init default full-screen address window:
    //ili9341_fsmc_setAddrWindow(0, 0, 320 - 1, 240 - 1); // CS_IDLE happens here
}

