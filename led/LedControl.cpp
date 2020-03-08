/*
 *    LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 * 
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    This permission notice shall be included in all copies or 
 *    substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */
 

#include "LedControl.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

LedControl::LedControl(int dataPin, int clkPin, int csPin, int numDevices) {
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    if(numDevices<=0 || numDevices>8 )
	numDevices=8;
    maxDevices=numDevices;
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);
    digitalWrite(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    for(int i=0;i<64;i++) 
	status[i]=0x00;
    for(int i=0;i<maxDevices;i++) {
	spiTransfer(i,OP_DISPLAYTEST,0);
	//scanlimit is set to max on startup
	setScanLimit(i,7);
	//decode is done in source
	spiTransfer(i,OP_DECODEMODE,0);
	clearDisplay(i);
	//we go into shutdown-mode on startup
	shutdown(i,true);
    }
}

int LedControl::getDeviceCount() {
    return maxDevices;
}

void LedControl::shutdown(int addr, bool b) {
    if(addr<0 || addr>=maxDevices)
	return;
    if(b)
	spiTransfer(addr, OP_SHUTDOWN,0);
    else
	spiTransfer(addr, OP_SHUTDOWN,1);
}
	
void LedControl::setScanLimit(int addr, int limit) {
    if(addr<0 || addr>=maxDevices)
	return;
    if(limit>=0 || limit<8)
    	spiTransfer(addr, OP_SCANLIMIT,limit);
}

void LedControl::setIntensity(int addr, int intensity) {
    if(addr<0 || addr>=maxDevices)
	return;
    if(intensity>=0 || intensity<16)	
	spiTransfer(addr, OP_INTENSITY,intensity);
    
}

void LedControl::clearDisplay(int addr) {
    int offset;

    if(addr<0 || addr>=maxDevices)
	return;
    offset=addr*8;
    for(int i=0;i<8;i++) {
	status[offset+i]=0;
	spiTransfer(addr, i+1,status[offset+i]);
    }
}

void LedControl::setLed(int addr, int row, int column, boolean state) {
    int offset;
    byte val=0x00;

    if(addr<0 || addr>=maxDevices)
	return;
    if(row<0 || row>7 || column<0 || column>7)
	return;
    offset=addr*8;
    val=B10000000 >> column;
    if(state)
	status[offset+row]=status[offset+row]|val;
    else {
	val=~val;
	status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}
	
void LedControl::setRow(int addr, int row, byte value) {
    int offset;
    if(addr<0 || addr>=maxDevices)
	return;
    if(row<0 || row>7)
	return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}
    
void LedControl::setColumn(int addr, int col, byte value) {
    byte val;

    if(addr<0 || addr>=maxDevices)
	return;
    if(col<0 || col>7) 
	return;
    for(int row=0;row<8;row++) {
	val=value >> (7-row);
	val=val & 0x01;
	setLed(addr,row,col,val);
    }
}

void LedControl::setDigit(int addr, int digit, byte value, boolean dp) {
    int offset;
    byte v;

    if(addr<0 || addr>=maxDevices)
	return;
    if(digit<0 || digit>7 || value>15)
	return;
    offset=addr*8;
    v=charTable[value];
    if(dp)
	v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
    
}

void LedControl::setChar(int addr, int digit, char value, boolean dp) {
    int offset;
    byte index,v;

    if(addr<0 || addr>=maxDevices)
	return;
    if(digit<0 || digit>7)
 	return;
    offset=addr*8;
    index=(byte)value;
    if(index >127) {
	//nothing define we use the space char
	value=32;
    }
    v=charTable[index];
    if(dp)
	v|=B10000000;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}

void LedControl::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
    //Create an array with the data to shift out
    int offset=addr*2;
    int maxbytes=maxDevices*2;

    for(int i=0;i<maxbytes;i++)
	spidata[i]=(byte)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line 
    digitalWrite(SPI_CS,LOW);
    //Now shift out the data 
    for(int i=maxbytes;i>0;i--)
 	shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
    //latch the data onto the display
    digitalWrite(SPI_CS,HIGH);
}    

//a partir daqui, editado por Yuri Crisostomo Bernardo
void LedControl::printChar(int addr, int pos, char c){
  
  byte sRow[7], i=0;
  sRow[0] = 0x00;
  sRow[6] = 0x00;
  
  switch (c){
    case ' ':
        sRow[1] = 0x00;
        sRow[2] = 0x00;
        sRow[3] = 0x00;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
        break;

    case '!':
        sRow[1] = 0x00;
        sRow[2] = 0x00;
        sRow[3] = 0x2f;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
        break;

    case '"':
        sRow[1] = 0x00;
        sRow[2] = 0x07;
        sRow[3] = 0x00;
        sRow[4] = 0x07;
        sRow[5] = 0x00;
        break;

    case '#':
        sRow[1] = 0x14;
        sRow[2] = 0x7f;
        sRow[3] = 0x14;
        sRow[4] = 0x7f;
        sRow[5] = 0x14;
        break;

    case '$':
        sRow[1] = 0x24;
        sRow[2] = 0x2a;
        sRow[3] = 0x7f;
        sRow[4] = 0x2a;
        sRow[5] = 0x12;
        break;

    case '%':
        sRow[1] = 0x62;
        sRow[2] = 0x64;
        sRow[3] = 0x08;
        sRow[4] = 0x13;
        sRow[5] = 0x23;
        break;

    case '&':
        sRow[1] = 0x36;
        sRow[2] = 0x49;
        sRow[3] = 0x55;
        sRow[4] = 0x22;
        sRow[5] = 0x50;
        break;

    case '(':
        sRow[1] = 0x00;
        sRow[2] = 0x1c;
        sRow[3] = 0x22;
        sRow[4] = 0x41;
        sRow[5] = 0x00;
        break;

    case ')':
        sRow[1] = 0x00;
        sRow[2] = 0x41;
        sRow[3] = 0x22;
        sRow[4] = 0x1c;
        sRow[5] = 0x00;
    break;

    case '*':
        sRow[1] = 0x14;
        sRow[2] = 0x08;
        sRow[3] = 0x3E;
        sRow[4] = 0x08;
        sRow[5] = 0x14;
    break;

    case '+':
        sRow[1] = 0x08;
        sRow[2] = 0x08;
        sRow[3] = 0x3E;
        sRow[4] = 0x08;
        sRow[5] = 0x08;
    break;

    case ',':
        sRow[1] = 0x00;
        sRow[2] = 0xA0;
        sRow[3] = 0x60;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
    break;

    case '-':
        sRow[1] = 0x08;
        sRow[2] = 0x08;
        sRow[3] = 0x08;
        sRow[4] = 0x08;
        sRow[5] = 0x08;
    break;

    case '.':
        sRow[1] = 0x00;
        sRow[2] = 0x60;
        sRow[3] = 0x60;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
    break;

    case '/':
        sRow[1] = 0x20;
        sRow[2] = 0x10;
        sRow[3] = 0x08;
        sRow[4] = 0x04;
        sRow[5] = 0x02;
    break;

    case '0':
        sRow[1] = 0x3E;
        sRow[2] = 0x51;
        sRow[3] = 0x49;
        sRow[4] = 0x45;
        sRow[5] = 0x3E;
    break;

    case '1':
        sRow[1] = 0x00;
        sRow[2] = 0x42;
        sRow[3] = 0x7F;
        sRow[4] = 0x40;
        sRow[5] = 0x00;
    break;

    case '2':
        sRow[1] = 0x42;
        sRow[2] = 0x61;
        sRow[3] = 0x51;
        sRow[4] = 0x49;
        sRow[5] = 0x46;
    break;

    case '3':
        sRow[1] = 0x21;
        sRow[2] = 0x41;
        sRow[3] = 0x45;
        sRow[4] = 0x4B;
        sRow[5] = 0x31;
    break;

    case '4':
        sRow[1] = 0x18;
        sRow[2] = 0x14;
        sRow[3] = 0x12;
        sRow[4] = 0x7F;
        sRow[5] = 0x10;
    break;

    case '5':
        sRow[1] = 0x27;
        sRow[2] = 0x45;
        sRow[3] = 0x45;
        sRow[4] = 0x45;
        sRow[5] = 0x39;
    break;

    case '6':
        sRow[1] = 0x3C;
        sRow[2] = 0x4A;
        sRow[3] = 0x49;
        sRow[4] = 0x49;
        sRow[5] = 0x30;
    break;

    case '7':
        sRow[1] = 0x01;
        sRow[2] = 0x71;
        sRow[3] = 0x09;
        sRow[4] = 0x05;
        sRow[5] = 0x03;
    break;

    case '8':
        sRow[1] = 0x36;
        sRow[2] = 0x49;
        sRow[3] = 0x49;
        sRow[4] = 0x49;
        sRow[5] = 0x36;
    break;

    case '9':
        sRow[1] = 0x06;
        sRow[2] = 0x49;
        sRow[3] = 0x49;
        sRow[4] = 0x29;
        sRow[5] = 0x1E;
    break;

    case ':':
        sRow[1] = 0x00;
        sRow[2] = 0x36;
        sRow[3] = 0x36;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
    break;

    case ';':
        sRow[1] = 0x00;
        sRow[2] = 0x56;
        sRow[3] = 0x36;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
    break;

    case '<':
        sRow[1] = 0x08;
        sRow[2] = 0x14;
        sRow[3] = 0x22;
        sRow[4] = 0x41;
        sRow[5] = 0x00;
    break;

    case '=':
        sRow[1] = 0x14;
        sRow[2] = 0x14;
        sRow[3] = 0x14;
        sRow[4] = 0x14;
        sRow[5] = 0x14;
    break;

    case '>':
        sRow[1] = 0x00;
        sRow[2] = 0x41;
        sRow[3] = 0x22;
        sRow[4] = 0x14;
        sRow[5] = 0x08;
    break;

    case '?':
        sRow[1] = 0x02;
        sRow[2] = 0x01;
        sRow[3] = 0x51;
        sRow[4] = 0x09;
        sRow[5] = 0x06;
    break;

    case '@':
        sRow[1] = 0x32;
        sRow[2] = 0x49;
        sRow[3] = 0x59;
        sRow[4] = 0x51;
        sRow[5] = 0x3E;
    break;

    case 'A':
        sRow[1] = 0x7C;
        sRow[2] = 0x12;
        sRow[3] = 0x11;
        sRow[4] = 0x12;
        sRow[5] = 0x7C;
    break;

    case 'B':
        sRow[1] = 0x7F;
        sRow[2] = 0x49;
        sRow[3] = 0x49;
        sRow[4] = 0x49;
        sRow[5] = 0x36;
    break;

    case 'C':
        sRow[1] = 0x3E;
        sRow[2] = 0x41;
        sRow[3] = 0x41;
        sRow[4] = 0x41;
        sRow[5] = 0x22;
    break;

    case 'D':
        sRow[1] = 0x7F;
        sRow[2] = 0x41;
        sRow[3] = 0x41;
        sRow[4] = 0x22;
        sRow[5] = 0x1C;
    break;

    case 'E':
        sRow[1] = 0x7F;
        sRow[2] = 0x49;
        sRow[3] = 0x49;
        sRow[4] = 0x49;
        sRow[5] = 0x41;
    break;

    case 'F':
        sRow[1] = 0x7F;
        sRow[2] = 0x09;
        sRow[3] = 0x09;
        sRow[4] = 0x09;
        sRow[5] = 0x01;
    break;

    case 'G':
        sRow[1] = 0x3E;
        sRow[2] = 0x41;
        sRow[3] = 0x49;
        sRow[4] = 0x49;
        sRow[5] = 0x7A;
    break;

    case 'H':
        sRow[1] = 0x7F;
        sRow[2] = 0x08;
        sRow[3] = 0x08;
        sRow[4] = 0x08;
        sRow[5] = 0x7F;
    break;

    case 'I':
        sRow[1] = 0x00;
        sRow[2] = 0x41;
        sRow[3] = 0x7F;
        sRow[4] = 0x41;
        sRow[5] = 0x00;
    break;

    case 'J':
        sRow[1] = 0x20;
        sRow[2] = 0x40;
        sRow[3] = 0x41;
        sRow[4] = 0x3F;
        sRow[5] = 0x01;
    break;

    case 'K':
        sRow[1] = 0x7F;
        sRow[2] = 0x08;
        sRow[3] = 0x14;
        sRow[4] = 0x22;
        sRow[5] = 0x41;
    break;

    case 'L':
        sRow[1] = 0x7F;
        sRow[2] = 0x40;
        sRow[3] = 0x40;
        sRow[4] = 0x40;
        sRow[5] = 0x40;
    break;

    case 'M':
        sRow[1] = 0x7F;
        sRow[2] = 0x02;
        sRow[3] = 0x0C;
        sRow[4] = 0x02;
        sRow[5] = 0x7F;
    break;

    case 'N':
        sRow[1] = 0x7F;
        sRow[2] = 0x04;
        sRow[3] = 0x08;
        sRow[4] = 0x10;
        sRow[5] = 0x7F;
    break;

    case 'O':
        sRow[1] = 0x3E;
        sRow[2] = 0x41;
        sRow[3] = 0x41;
        sRow[4] = 0x41;
        sRow[5] = 0x3E;
    break;

    case 'P':
        sRow[1] = 0x7F;
        sRow[2] = 0x09;
        sRow[3] = 0x09;
        sRow[4] = 0x09;
        sRow[5] = 0x06;
    break;

    case 'Q':
        sRow[1] = 0x3E;
        sRow[2] = 0x41;
        sRow[3] = 0x51;
        sRow[4] = 0x21;
        sRow[5] = 0x5E;
    break;

    case 'R':
        sRow[1] = 0x7F;
        sRow[2] = 0x09;
        sRow[3] = 0x19;
        sRow[4] = 0x29;
        sRow[5] = 0x46;
    break;

    case 'S':
        sRow[1] = 0x46;
        sRow[2] = 0x49;
        sRow[3] = 0x49;
        sRow[4] = 0x49;
        sRow[5] = 0x31;
    break;

    case 'T':
        sRow[1] = 0x01;
        sRow[2] = 0x01;
        sRow[3] = 0x7F;
        sRow[4] = 0x01;
        sRow[5] = 0x01;
    break;

    case 'U':
        sRow[1] = 0x3F;
        sRow[2] = 0x40;
        sRow[3] = 0x40;
        sRow[4] = 0x40;
        sRow[5] = 0x3F;
    break;

    case 'V':
        sRow[1] = 0x1F;
        sRow[2] = 0x20;
        sRow[3] = 0x40;
        sRow[4] = 0x20;
        sRow[5] = 0x1F;
    break;

    case 'W':
        sRow[1] = 0x3F;
        sRow[2] = 0x40;
        sRow[3] = 0x38;
        sRow[4] = 0x40;
        sRow[5] = 0x3F;
    break;

    case 'X':
        sRow[1] = 0x63;
        sRow[2] = 0x14;
        sRow[3] = 0x08;
        sRow[4] = 0x14;
        sRow[5] = 0x63;
    break;

    case 'Y':
        sRow[1] = 0x07;
        sRow[2] = 0x08;
        sRow[3] = 0x70;
        sRow[4] = 0x08;
        sRow[5] = 0x07;
    break;

    case 'Z':
        sRow[1] = 0x61;
        sRow[2] = 0x51;
        sRow[3] = 0x49;
        sRow[4] = 0x45;
        sRow[5] = 0x43;
    break;

    case '[':
        sRow[1] = 0x00;
        sRow[2] = 0x7F;
        sRow[3] = 0x41;
        sRow[4] = 0x41;
        sRow[5] = 0x00;
    break;

    case '\\':
        sRow[1] = 0x55;
        sRow[2] = 0xAA;
        sRow[3] = 0x55;
        sRow[4] = 0xAA;
        sRow[5] = 0x55;
    break;

    case ']':
        sRow[1] = 0x00;
        sRow[2] = 0x41;
        sRow[3] = 0x41;
        sRow[4] = 0x7F;
        sRow[5] = 0x00;
    break;

    case '^':
        sRow[1] = 0x04;
        sRow[2] = 0x02;
        sRow[3] = 0x01;
        sRow[4] = 0x02;
        sRow[5] = 0x04;
    break;

    case '_':
        sRow[1] = 0x40;
        sRow[2] = 0x40;
        sRow[3] = 0x40;
        sRow[4] = 0x40;
        sRow[5] = 0x40;
    break;

    case '`':
        sRow[1] = 0x00;
        sRow[2] = 0x03;
        sRow[3] = 0x05;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
    break;

    case 'a':
        sRow[1] = 0x20;
        sRow[2] = 0x54;
        sRow[3] = 0x54;
        sRow[4] = 0x54;
        sRow[5] = 0x78;
    break;

    case 'b':
        sRow[1] = 0x7F;
        sRow[2] = 0x48;
        sRow[3] = 0x44;
        sRow[4] = 0x44;
        sRow[5] = 0x38;
    break;

    case 'c':
        sRow[1] = 0x38;
        sRow[2] = 0x44;
        sRow[3] = 0x44;
        sRow[4] = 0x44;
        sRow[5] = 0x20;
    break;

    case 'd':
        sRow[1] = 0x38;
        sRow[2] = 0x44;
        sRow[3] = 0x44;
        sRow[4] = 0x48;
        sRow[5] = 0x7F;
    break;

    case 'e':
        sRow[1] = 0x38;
        sRow[2] = 0x54;
        sRow[3] = 0x54;
        sRow[4] = 0x54;
        sRow[5] = 0x18;
    break;

    case 'f':
        sRow[1] = 0x08;
        sRow[2] = 0x7E;
        sRow[3] = 0x09;
        sRow[4] = 0x01;
        sRow[5] = 0x02;
    break;

    case 'g':
        sRow[1] = 0x18;
        sRow[2] = 0xA4;
        sRow[3] = 0xA4;
        sRow[4] = 0xA4;
        sRow[5] = 0x7C;
    break;

    case 'h':
        sRow[1] = 0x7F;
        sRow[2] = 0x08;
        sRow[3] = 0x04;
        sRow[4] = 0x04;
        sRow[5] = 0x78;
    break;

    case 'i':
        sRow[1] = 0x00;
        sRow[2] = 0x44;
        sRow[3] = 0x7D;
        sRow[4] = 0x40;
        sRow[5] = 0x00;
    break;

    case 'j':
        sRow[1] = 0x40;
        sRow[2] = 0x80;
        sRow[3] = 0x84;
        sRow[4] = 0x7D;
        sRow[5] = 0x00;
    break;

    case 'k':
        sRow[1] = 0x7F;
        sRow[2] = 0x10;
        sRow[3] = 0x28;
        sRow[4] = 0x44;
        sRow[5] = 0x00;
    break;

    case 'l':
        sRow[1] = 0x00;
        sRow[2] = 0x41;
        sRow[3] = 0x7F;
        sRow[4] = 0x40;
        sRow[5] = 0x00;
    break;

    case 'm':
        sRow[1] = 0x7C;
        sRow[2] = 0x04;
        sRow[3] = 0x18;
        sRow[4] = 0x04;
        sRow[5] = 0x78;
    break;

    case 'n':
        sRow[1] = 0x7C;
        sRow[2] = 0x08;
        sRow[3] = 0x04;
        sRow[4] = 0x04;
        sRow[5] = 0x78;
    break;

    case 'o':
        sRow[1] = 0x38;
        sRow[2] = 0x44;
        sRow[3] = 0x44;
        sRow[4] = 0x44;
        sRow[5] = 0x38;
    break;

    case 'p':
        sRow[1] = 0xFC;
        sRow[2] = 0x24;
        sRow[3] = 0x24;
        sRow[4] = 0x24;
        sRow[5] = 0x18;
    break;

    case 'q':
        sRow[1] = 0x18;
        sRow[2] = 0x24;
        sRow[3] = 0x24;
        sRow[4] = 0x18;
        sRow[5] = 0xFC;
    break;

    case 'r':
        sRow[1] = 0x7C;
        sRow[2] = 0x08;
        sRow[3] = 0x04;
        sRow[4] = 0x04;
        sRow[5] = 0x08;
    break;

    case 's':
        sRow[1] = 0x48;
        sRow[2] = 0x54;
        sRow[3] = 0x54;
        sRow[4] = 0x54;
        sRow[5] = 0x20;
    break;

    case 't':
        sRow[1] = 0x04;
        sRow[2] = 0x3F;
        sRow[3] = 0x44;
        sRow[4] = 0x40;
        sRow[5] = 0x20;
    break;

    case 'u':
        sRow[1] = 0x3C;
        sRow[2] = 0x40;
        sRow[3] = 0x40;
        sRow[4] = 0x20;
        sRow[5] = 0x7C;
    break;

    case 'v':
        sRow[1] = 0x1C;
        sRow[2] = 0x20;
        sRow[3] = 0x40;
        sRow[4] = 0x20;
        sRow[5] = 0x1C;
    break;

    case 'w':
        sRow[1] = 0x3C;
        sRow[2] = 0x40;
        sRow[3] = 0x30;
        sRow[4] = 0x40;
        sRow[5] = 0x3C;
    break;

    case 'x':
        sRow[1] = 0x44;
        sRow[2] = 0x28;
        sRow[3] = 0x10;
        sRow[4] = 0x28;
        sRow[5] = 0x44;
    break;

    case 'y':
        sRow[1] = 0x1C;
        sRow[2] = 0xA0;
        sRow[3] = 0xA0;
        sRow[4] = 0xA0;
        sRow[5] = 0x7C;
    break;

    case 'z':
        sRow[1] = 0x44;
        sRow[2] = 0x64;
        sRow[3] = 0x54;
        sRow[4] = 0x4C;
        sRow[5] = 0x44;
    break;

    case '{':
        sRow[1] = 0x00;
        sRow[2] = 0x10;
        sRow[3] = 0x7C;
        sRow[4] = 0x82;
        sRow[5] = 0x00;
    break;

    case '|':
        sRow[1] = 0x00;
        sRow[2] = 0x00;
        sRow[3] = 0xFF;
        sRow[4] = 0x00;
        sRow[5] = 0x00;
    break;

    case '}':
        sRow[1] = 0x00;
        sRow[2] = 0x82;
        sRow[3] = 0x7C;
        sRow[4] = 0x10;
        sRow[5] = 0x00;
    break;

    case 'º':
        sRow[1] = 0x00;
        sRow[2] = 0x06;
        sRow[3] = 0x09;
        sRow[4] = 0x09;
        sRow[5] = 0x06;
    break;   
  }
  
  for (i=0; i<7; i++){
    LedControl::setRow(addr, pos+i, sRow[i]);
  }
  
 }

void LedControl::printString(int addr, int pos, char string[]){
  
  int i=0;
  
  while (string[i] != '\0'){
    printChar(addr, 6*i+pos, string[i]);
    i++;
  }
  
}


void LedControl::printStringScroll(int addr, int pos, char string[], int tDelay, char sentido){
  
  int i=0, c=0;
  
  while (string[c] != '\0'){
    c++;
  }
  
  if (sentido == '<'){
    
    for (i=0; i<((c*6)+1); i++){
      printString(addr, -i+pos, string);
      delay(tDelay);
    }
    
  }else if (sentido == '>'){
    
    for (i=0; i<((c*6)+1); i++){
      printString(addr, (-(c*6)+i)+pos, string);
      delay(tDelay);
    }
    
  }
}
