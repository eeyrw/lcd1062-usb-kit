/*
  ST7032.cpp - Arduino LiquidCrystal compatible library

  Original source is Arduino LiquidCrystal liblary

  Author:  tomozh@gmail.com
  License: MIT
  History:
    2014.10.13 コントラスト値のbit7がBONビットに影響する不具合を修正
    2014.08.23 コンストラクタでI2Cアドレスを設定可能にした
    2013.05.21 1st release

 ------------------------
  Arduino        ST7032i
 ------------------------
  3.3V    --+-- VDD
            +-- -RES
  A4(SDA) --*-- SDA
  A5(SCL) --*-- SCL
  GND     ----- GND

  *... 10Kohm pull-up
 ------------------------
*/

#include "ST7032.h"
#include "fw_hal.h"
// private methods

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
//  uint8_t _iconfunction;

uint8_t _initialized;

uint8_t _numlines;
uint8_t _currline;

uint8_t _i2c_addr;

void setDisplayControl(uint8_t setBit)
{
    _displaycontrol |= setBit;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void resetDisplayControl(uint8_t resetBit)
{
    _displaycontrol &= ~resetBit;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void setEntryMode(uint8_t setBit)
{
    _displaymode |= setBit;
    command(LCD_ENTRYMODESET | _displaymode);
}

void resetEntryMode(uint8_t resetBit)
{
    _displaymode &= ~resetBit;
    command(LCD_ENTRYMODESET | _displaymode);
}

void normalFunctionSet()
{
    command(LCD_FUNCTIONSET | _displayfunction);
}

void extendFunctionSet()
{
    command(LCD_FUNCTIONSET | _displayfunction | LCD_EX_INSTRUCTION);
}

// public methods

void st7032_init(uint8_t i2c_addr)
{
    GPIO_P1_SetMode(GPIO_Pin_6, GPIO_Mode_Output_PP);
    P16 =0;
    SYS_DelayUs(200);    
    P16 =1;
    _displaycontrol = 0x00;
    _displaymode = 0x00;
    _i2c_addr = i2c_addr;

    // Master mode
    I2C_SetWorkMode(I2C_WorkMode_Master);
    /**
     * I2C clock = FOSC / 2 / (__prescaler__ * 2 + 4)
     * SSD1306 works with i2c clock up to 1.3 MHz, beyond this value, display may fail.
     */
    I2C_SetClockPrescaler(0x10);
    // Switch alternative port
    I2C_SetPort(I2C_AlterPort_P15_P14);
    // Start I2C
    I2C_SetEnabled(HAL_State_ON);

    // SDA
    GPIO_P1_SetMode(GPIO_Pin_4, GPIO_Mode_InOut_OD);
    // SCL
    GPIO_P1_SetMode(GPIO_Pin_5, GPIO_Mode_InOut_OD);





}

void begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{

    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    if (lines > 1)
    {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = lines;
    _currline = 0;

    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != 0) && (lines == 1))
    {
        _displayfunction |= LCD_5x10DOTS;
    }

    // Wire.begin();
    SYS_Delay(40); // Wait time >40ms After VDD stable

    // finally, set # lines, font size, etc.
    normalFunctionSet();

    extendFunctionSet();
    command(LCD_EX_SETBIASOSC | LCD_BIAS_1_5 | LCD_OSC_183HZ);        // 1/5bias, OSC=183Hz@3.0V
    command(LCD_EX_FOLLOWERCONTROL | LCD_FOLLOWER_ON | LCD_RAB_2_00); // internal follower circuit is turn on
    // delay(200);                                                       // Wait time >200ms (for power stable)
    normalFunctionSet();

    // turn the display on with no cursor or blinking default
    //  display();
    _displaycontrol = 0x00; // LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    setDisplayControl(LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);

    // clear it off
    clear();

    // Initialize to default text direction (for romance languages)
    //  command(LCD_ENTRYMODESET | _displaymode);
    _displaymode = 0x00; // LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    setEntryMode(LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
}

void setContrast(uint8_t cont)
{
    extendFunctionSet();
    command(LCD_EX_CONTRASTSETL | (cont & 0x0f));                                         // Contrast set
    command(LCD_EX_POWICONCONTRASTH | LCD_ICON_ON | LCD_BOOST_ON | ((cont >> 4) & 0x03)); // Power, ICON, Contrast control
    normalFunctionSet();
}

void setIcon(uint8_t addr, uint8_t bit)
{
    extendFunctionSet();
    command(LCD_EX_SETICONRAMADDR | (addr & 0x0f)); // ICON address
    write(bit);
    normalFunctionSet();
}

/********** high level commands, for the user! */
void clear()
{
    command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
    // delayMicroseconds(2000);   // this command takes a long time!
}

void home()
{
    command(LCD_RETURNHOME); // set cursor position to zero
    // delayMicroseconds(2000); // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row)
{
    const int row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    if (row > _numlines)
    {
        row = _numlines - 1; // we count rows starting w/0
    }

    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay()
{
    resetDisplayControl(LCD_DISPLAYON);
}
void display()
{
    setDisplayControl(LCD_DISPLAYON);
}

// Turns the underline cursor on/off
void noCursor()
{
    resetDisplayControl(LCD_CURSORON);
}
void cursor()
{
    setDisplayControl(LCD_CURSORON);
}

// Turn on and off the blinking cursor
void noBlink()
{
    resetDisplayControl(LCD_BLINKON);
}
void blink()
{
    setDisplayControl(LCD_BLINKON);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void)
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void scrollDisplayRight(void)
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void)
{
    setEntryMode(LCD_ENTRYLEFT);
}

// This is for text that flows Right to Left
void rightToLeft(void)
{
    resetEntryMode(LCD_ENTRYLEFT);
}

// This will 'right justify' text from the cursor
void autoscroll(void)
{
    setEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void)
{
    resetEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(uint8_t location, uint8_t charmap[])
{
    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
    {
        write(charmap[i]);
    }
}

/*********** mid level commands, for sending data/cmds */

void command(uint8_t value)
{
    // Wire.beginTransmission(_i2c_addr);
    // Wire.write((uint8_t)0x00);
    // Wire.write(value);
    // Wire.endTransmission();
    // delayMicroseconds(27); // >26.3us
    I2C_Write(_i2c_addr, 0x00, &value, 1);
    SYS_DelayUs(27);
}

int write(uint8_t value)
{
    // Wire.beginTransmission(_i2c_addr);
    // Wire.write((uint8_t)0x40);
    // Wire.write(value);
    // Wire.endTransmission();
    // delayMicroseconds(27); // >26.3us
    I2C_Write(_i2c_addr, 0x40, &value, 1);
    SYS_DelayUs(27);
    return 1;
}