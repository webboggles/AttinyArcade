/* 2017
 * Run Dude Run game by Ilya Titov. Find building instructions on http://webboggles.com/
 * The code that does not fall under the licenses of sources listed below can be used non commercially with attribution.
 *
 * If you have problems uploading this sketch, this is probably due to sketch size - you need to update ld.exe in arduino\hardware\tools\avr\avr\bin
 * https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC
 *
 * This sketch is using the screen control and font functions written by Neven Boyanov for the http://tinusaur.wordpress.com/ project
 * Source code and font files available at: https://bitbucket.org/tinusaur/ssd1306xled
 * 
 * Sleep code is based on this blog post by Matthew Little:
 * http://www.re-innovation.co.uk/web12/index.php/en/blog-75/306-sleep-modes-on-attiny85
*/
#include <EEPROM.h>
#include "font6x8.h"
#include <avr/sleep.h>
#include <avr/interrupt.h> // needed for the additional interrupt

#define DIGITAL_WRITE_HIGH(PORT) PORTB |= (1 << PORT)
#define DIGITAL_WRITE_LOW(PORT) PORTB &= ~(1 << PORT)

// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358
#ifndef SSD1306XLED_H
#define SSD1306XLED_H
// ---------------------	// Vcc,	Pin 1 on SSD1306 Board
// ---------------------	// GND,	Pin 2 on SSD1306 Board
#ifndef SSD1306_SCL
#define SSD1306_SCL		PB4	// SCL,	Pin 3 on SSD1306 Board
#endif
#ifndef SSD1306_SDA
#define SSD1306_SDA		PB3	// SDA,	Pin 4 on SSD1306 Board
#endif
#ifndef SSD1306_SA
#define SSD1306_SA		0x78	// Slave address
#endif
// ----------------------------------------------------------------------------
void ssd1306_init(void);
void ssd1306_xfer_start(void);
void ssd1306_xfer_stop(void);
void ssd1306_send_byte(uint8_t byte);
void ssd1306_send_command(uint8_t command);
void ssd1306_send_data_start(void);
void ssd1306_send_data_stop(void);
void ssd1306_setpos(uint8_t x, uint8_t y);
void ssd1306_fillscreen(uint8_t fill_Data);
void ssd1306_char_f6x8(uint8_t x, uint8_t y, const char ch[]);
//void ssd1306_char_f8x16(uint8_t x, uint8_t y,const char ch[]);
//void ssd1306_char_f16x16(uint8_t x, uint8_t y, uint8_t N);
void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t bitmap[]);
// ----------------------------------------------------------------------------
#endif

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


const uint8_t playerIdle1 [] PROGMEM =  { // inverted, bottom first
B00111100,
B11011011,
B11000010,
B11011011,
B00111100
};
const uint8_t playerIdle2 [] PROGMEM =  {
B00111011,
B11010111,
B11000101,
B11010111,
B00111011
};
const uint8_t playerRight1 [] PROGMEM =  {
B11110111,
B01111011,
B10011011,
B00000010,
B01101111
};
const uint8_t playerRight2 [] PROGMEM =  {
B00111111,
B11010011,
B11100010,
B00011011,
B11111111
};
const uint8_t playerLeft1 [] PROGMEM =  {
B01101111,
B00000010,
B10011011,
B01111011,
B11110111
};
const uint8_t playerLeft2 [] PROGMEM =  {
B11111111,
B00011011,
B11100010,
B11010011,
B00111111
};
const uint8_t bomb1 [] PROGMEM =  {
B11111111,
B10001111,
B01111111,
B00011111,
B01111111,
B10001111,
B11111111,
B11000111,
B10111001,
B01111110,
B01111100,
B00010000,
B10000001,
B11000111
};

const uint8_t bottle1 [] PROGMEM =  {
B11111111,
B11101111,
B00001011,
B00000011,
B11101111,
B11111111,
B10000001,
B01011010,
B01011011,
B00011000,
B00011000,
B10000001
};

const uint8_t splash  [] PROGMEM =  {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
0x3E, 0xFE, 0xE2, 0xC2, 0xFE, 0x80, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x00, 0x00, 0xC0, 0xC0, 0x80,
0x00, 0x20, 0x70, 0xC0, 0xE0, 0x30, 0x70, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x60, 0x1E, 0xFE, 0xF0, 0xF8, 0xF7, 0xFB, 0xFC, 0xF8, 0xF8, 0xFE,
0xFD, 0xFC, 0xF7, 0xFC, 0xFC, 0xFE, 0xFD, 0xFF, 0xFC, 0xFC, 0xF4, 0xFC, 0xF4, 0xFE, 0xFE, 0xFE,
0xDC, 0xF8, 0xFF, 0xFF, 0xF8, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x07, 0xEF, 0xE0, 0x61, 0x61, 0x73, 0x76, 0x70, 0x73, 0x77, 0xF6, 0xE6, 0xE7, 0xE3,
0xE6, 0xC0, 0xC0, 0x01, 0xC3, 0xB3, 0xE0, 0x81, 0x03, 0x03, 0x00, 0x00, 0x10, 0xF8, 0xF0, 0xE0,
0x84, 0x1C, 0x7C, 0xFC, 0xFE, 0xC6, 0x06, 0x0E, 0x0A, 0x1C, 0x74, 0x68, 0xB0, 0xE0, 0x86, 0x7F,
0xFB, 0xF3, 0xB3, 0x33, 0x1B, 0x1A, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x83, 0x00, 0xFF, 0x3F, 0xFF, 0xFF, 0x7E, 0x3F, 0x0F, 0x07, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00,
0x00, 0x04, 0x04, 0x04, 0x84, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x0F, 0x1F, 0x1F, 0x1E, 0x7B, 0x7F,
0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0x00, 0x00, 0x00, 0x10, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x60, 0xE0, 0xA0, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFC, 0xF8,
0x00, 0x0E, 0x3C, 0x7F, 0xFC, 0xF0, 0xE0, 0x90, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x0F, 0xFF, 0xFF, 0xFC, 0x71, 0x07, 0x0F, 0x3F, 0xFC, 0xF0, 0xE0, 0x80, 0xE0, 0x3F, 0x01,
0x03, 0x0F, 0x3C, 0xF8, 0x63, 0x4F, 0x1E, 0x7C, 0xE0, 0xC0, 0xE0, 0x60, 0x3F, 0x1E, 0x0F, 0x00,
0x01, 0x07, 0x07, 0x0F, 0x0E, 0x0C, 0x1C, 0x1A, 0x1A, 0x0E, 0x84, 0x00, 0x00, 0x00, 0x20, 0xFF,
0xFF, 0xFE, 0x7F, 0x3F, 0x1F, 0x0B, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
0x80, 0xC0, 0xC0, 0xC0, 0x80, 0xC1, 0xC1, 0x81, 0x81, 0x03, 0x01, 0x02, 0x02, 0x06, 0x0C, 0x0C,
0x08, 0x00, 0x01, 0x10, 0x90, 0xA0, 0xBF, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x7C, 0x08, 0x00, 0x00,
0x80, 0x80, 0x00, 0x03, 0x0F, 0x3E, 0x7E, 0xF3, 0xC3, 0x86, 0x0C, 0xB8, 0xE0, 0xFF, 0x81, 0xFF,
0x80, 0x80, 0x80, 0x80, 0x81, 0x03, 0x0F, 0x3F, 0x7B, 0xFC, 0xF8, 0xF0, 0xF0, 0xF8, 0x78, 0x7C,
0x36, 0x1F, 0x0D, 0x07, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0x30, 0x71, 0x63, 0xC1, 0x00, 0x00, 0x00,
0x00, 0x00, 0xC0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x3C,
0xF8, 0xF8, 0x70, 0xE0, 0xC0, 0x80, 0x06, 0xFE, 0xFE, 0x00, 0x07, 0x3C, 0xE0, 0x00, 0x00, 0x1F,
0x07, 0x00, 0xE0, 0xEC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
0x03, 0x03, 0x0F, 0x3F, 0x3B, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x07, 0x07, 0x0E, 0xFE, 0xFE,
0xFF, 0xFF, 0xBF, 0xFC, 0xFC, 0xF8, 0xF0, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
0x0F, 0x1F, 0x3F, 0x3E, 0x3E, 0x5E, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB,
0xF3, 0x83, 0x03, 0x03, 0x03, 0x03, 0x83, 0xC3, 0x36, 0x1C, 0x9F, 0xBB, 0xB1, 0xE0, 0xC0, 0xE0,
0x20, 0x20, 0x20, 0x40, 0x40, 0x63, 0x27, 0xFF, 0xBF, 0xFC, 0xE0, 0xFC, 0x3F, 0x3F, 0x30, 0x70,
0x70, 0x70, 0xE1, 0xE3, 0xE7, 0x5E, 0x7C, 0x70, 0x60, 0x3F, 0x1F, 0x07, 0x0F, 0x1F, 0x1C, 0x38,
0x01, 0x1F, 0x3F, 0x3C, 0x03, 0x07, 0x0F, 0x0F, 0x0F, 0x04, 0x00, 0x00, 0x0D, 0x0C, 0x84, 0xA0,
0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0xE0, 0xE0, 0xC0, 0xE0, 0xE0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC,
0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xF1, 0xF3, 0xE2, 0xE0, 0xC0, 0xC0, 0xE0, 0xE0, 0xC1, 0xF9,
0xF3, 0xE3, 0xC3, 0x83, 0x83, 0x03, 0x07, 0xFC, 0x00, 0x00, 0x10, 0x30, 0x38, 0x04, 0x04, 0x0C,
0x42, 0x46, 0x84, 0xCC, 0x7E, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0xBF, 0x3D,
0x7F, 0xFD, 0x13, 0xDE, 0x9C, 0x0E, 0x09, 0x1C, 0x06, 0xC3, 0xF1, 0x50, 0xDC, 0x04, 0x05, 0x07,
0x0F, 0x5E, 0xF8, 0xF0, 0xF0, 0x90, 0x20, 0x60, 0xC3, 0x86, 0x8F, 0x6F, 0x39, 0x0C, 0x06, 0x0E,
0x08, 0x04, 0xC4, 0x7C, 0xF0, 0x8C, 0x04, 0x00, 0x00, 0x00, 0x38, 0x2E, 0x76, 0xF8, 0xD8, 0xF8,
0xCC, 0xE4, 0x64, 0x24, 0x00, 0x18, 0x1C, 0x0E, 0x06, 0x02, 0x00, 0x03, 0x01, 0xC0, 0xFB, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x8F, 0xCF,
0xE7, 0xE7, 0xF7, 0xF3, 0xF3, 0xF3, 0xF3, 0xC3, 0xE7, 0xC7, 0x87, 0x8F, 0x0F, 0x1F, 0x7F, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x18, 0x0F, 0x80, 0xC4, 0x88, 0x00, 0x30, 0x30, 0x70, 0xE0,
0x28, 0x2C, 0x27, 0x21, 0x23, 0x45, 0x49, 0x73, 0x03, 0xF7, 0xEF, 0xE8, 0x63, 0x9F, 0xDF, 0x9F,
0x7E, 0x7D, 0x3C, 0xCF, 0xF3, 0xF9, 0xF2, 0xC7, 0xC7, 0xCF, 0xC4, 0x82, 0x8F, 0xEF, 0xB2, 0xF3,
0x73, 0xC9, 0xDB, 0x4F, 0x5F, 0x7D, 0xDF, 0xC2, 0xC3, 0xF3, 0x7F, 0xFF, 0xFF, 0xFF, 0xFC, 0xBE,
0x2A, 0x37, 0x9C, 0x8E, 0x1B, 0x19, 0x59, 0xB3, 0xFC, 0xDD, 0x5A, 0xEE, 0xE7, 0x6B, 0x15, 0x91,
0x80, 0xC1, 0xC3, 0xC0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xE2, 0xEF, 0xCF, 0x8F,
0x8F, 0x0F, 0x07, 0x3F, 0x3F, 0x3F, 0xFF, 0x7F, 0x07, 0xF3, 0xF9, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0x7F, 0xF7, 0xF3, 0xE1, 0xF0, 0xD8, 0xD8, 0xFC, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0x7F,
0x18, 0x2F, 0x6D, 0xE1, 0x3E, 0x01, 0x81, 0xD9, 0xF3, 0xE3, 0xE1, 0xE4, 0xE6, 0xEB, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x67, 0xE7, 0x8F, 0x9F, 0xE7, 0xF9, 0xFC, 0xFD, 0xFF, 0xFC,
0xF9, 0xF2, 0xF3, 0xFC, 0xFC, 0xFE, 0x7E, 0x7E, 0xFF, 0xFF, 0xF8, 0xF3, 0xFA, 0xF8, 0xFC, 0xF8,
0xFD, 0xF1, 0xC6, 0xCE, 0x0E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFA, 0xF9, 0xF9,
0xFE, 0xBD, 0x7B, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7E, 0x7E, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xFF, 0x7F, 0x3F, 0x1F, 0x1F, 0x1F, 0x1F,
0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xF7, 0xE7, 0xE3, 0xFF, 0xFF, 0xFF, 0xFD, 0xFC, 0x6C
};

unsigned long activeMillis = 0;
byte bombX[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte bombY[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte bombStatus[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte totalBombs = 1;
byte maxBombs = 16;
byte playerOffset = 62;
byte playerStatus = 0; // 0 idle, 1 right, 2 left
byte quadFrameCount = 0;
boolean btn1 = 0;
boolean btn2 = 0;
unsigned int score = 0;
unsigned long lastFrame = 0;
unsigned long lastBomb = 0;
int bombDelay = 1500;
int frameDelay = 40;
byte playerStep = 2;
boolean stopAnimate = 0;

ISR(PCINT0_vect){ // PB0 pin button interrupt			     
   btn1=1;
   
   activeMillis = millis();
   return;
}
void playerInc(){ // PB2 pin button interrupt
  btn2 = 1;
  
  activeMillis = millis();
}

void setup() {

  DDRB = 0b00000010;  	// saet PB1 as output (for the speaker)
  PCMSK = 0b00000001;	// pin change mask: listen to portb bit 1
  GIMSK |= 0b00100000;	// enable PCINT interrupt 
  sei();			// enable all interrupts
  attachInterrupt(0,playerInc,CHANGE);
  //pinMode(1, OUTPUT);
  digitalWrite(1,HIGH);
  activeMillis = millis();
  ssd1306_init();
  resetGame();
  
  for (byte tb = 1; tb <= totalBombs; tb++){
   bombY[tb]=random(0,64); 
  }
}
void loop() { 

      
     
     // ssd1306_fillscreen(0x00);
   while (1==1){
     if (frameDelay+lastFrame < millis()){
              lastFrame = millis();
              playerStatus = 0;
              if (digitalRead(0)==1||btn1==1){
                playerStatus = 2;
                if (playerOffset >0){
                   playerOffset -=playerStep;
                }
                btn1 = 0;
              }
              if (digitalRead(2)==1||btn2==1){
                playerStatus = 1;
                if (playerOffset < 121){
                  playerOffset +=playerStep;
                }
                btn2 = 0;
              }
              
              
              drawPlayer();
              drawBombs();
              quadFrameCount++;
              if (quadFrameCount>=3){quadFrameCount = 0;}   
                  
                  
                  
                  //ssd1306_draw_bmp(14,0,21,2,bomb1);
                 // delay(150);
                  
                 //ssd1306_draw_bmp(22,2,28,4,bottle1);
                  //delay(150);
             //     
             
             // display score
                if (stopAnimate==1){
                 int topScore = EEPROM.read(0);
                    topScore = topScore << 8;
                    topScore = topScore |  EEPROM.read(1);
                    if (topScore<0){EEPROM.write(1,0 & 0xFF); EEPROM.write(0,(0>>8) & 0xFF);}
                    if (score>topScore){topScore = score; EEPROM.write(1,topScore & 0xFF); EEPROM.write(0,(topScore>>8) & 0xFF); }

                    ssd1306_char_f6x8(32, 3, "Game Over");
                    ssd1306_char_f6x8(32, 5, "score:");
                    char temp[10] = {0,0,0,0,0,0,0,0,0,0};
                    itoa(score,temp,10);
                    ssd1306_char_f6x8(70, 5, temp);
                    ssd1306_char_f6x8(32, 6, "top score:");
                    itoa(topScore,temp,10);
                    ssd1306_char_f6x8(90, 6, temp);
                    for (int i = 0; i<1000; i++){
                      beep(1,random(0,i*2));
                    }
                    delay(2000);
                   
                    system_sleep();
                    resetGame();
                     
                }
                
                char temp[10] = {0,0,0,0,0,0,0,0,0,0};
                itoa(score,temp,10);
                ssd1306_char_f6x8(92, 0, temp);
                
               if (activeMillis+30000<millis()||stopAnimate==1){
                system_sleep();
               }
               
       } 
     }	
}
void resetGame(){
    ssd1306_char_f6x8(18, 4, "webboggles.com");
    delay(200);
    ssd1306_char_f6x8(12, 6, "Tweet @webboggles");  
    ssd1306_char_f6x8(22, 7, "#AttinyArcade");  
    delay(1000);
    ssd1306_draw_bmp(0,0,128,8,splash);
    delay(2000);

    
    ssd1306_fillscreen(B00000000);
    
    stopAnimate=0;
    for (byte i = 0; i<16; i++){
      bombX[i] = 0;
      bombY[i] = 0;
      bombStatus[i] = 0;
    }
    totalBombs = 1;
    bombDelay = 1500;
    frameDelay = 40;
    score = 0;
}
void drawBombs(){
  if (lastBomb+bombDelay<millis()){
    lastBomb = millis();
     
    if (score < 10 && score > 0){
       totalBombs++; 
    }else if (score>100&&score<1000){
      bombDelay = 3000;
      if (frameDelay>0){frameDelay--;}
    }else if (score > 1000){
      if (totalBombs<maxBombs){
       totalBombs++;
      } 
    }
  }
  for (byte bi = 0; bi<totalBombs; bi++){
    if (bombStatus[bi] != 1){
      bombStatus[bi] = 1;
      bombX[bi]=random(0,120);
      bombY[bi]=0;
    }
  }
    
  for (byte tb = 0; tb < totalBombs; tb++){
    if (bombStatus[tb]==1){
       bombY[tb] ++;      if (bombY[tb]>=63){bombY[tb]=0;}  // remove later
       byte offset = 8- bombY[tb]%8;
       byte offset2 =  bombY[tb]%8;
       
      
       if (bombY[tb]>0){
              ssd1306_setpos(bombX[tb],bombY[tb]/8);
              ssd1306_send_data_start();
              ssd1306_send_byte((~bomb1[7])>>offset^B11111111<<offset2);
              ssd1306_send_byte((~bomb1[8])>>offset^B11111111<<offset2);
              ssd1306_send_byte((~bomb1[9])>>offset^B11111111<<offset2);
              ssd1306_send_byte((~bomb1[10])>>offset^B11111111<<offset2);
              ssd1306_send_byte((~bomb1[11])>>offset^B11111111<<offset2);
              ssd1306_send_byte((~bomb1[12])>>offset^B11111111<<offset2);
              ssd1306_send_byte((~bomb1[13])>>offset^B11111111<<offset2);
              ssd1306_send_data_stop();
              
       }
       
       if (bombY[tb]>=8){
              ssd1306_setpos(bombX[tb],bombY[tb]/8-1);
              ssd1306_send_data_start();
              ssd1306_send_byte((((~bomb1[0])>>offset)^B11111111<<offset2)|(~bomb1[7])<<offset2);
              ssd1306_send_byte((((~bomb1[1])>>offset)^B11111111<<offset2)|(~bomb1[8])<<offset2);
              ssd1306_send_byte((((~bomb1[2])>>offset)^B11111111<<offset2)|(~bomb1[9])<<offset2);
              ssd1306_send_byte((((~bomb1[3])>>offset)^B11111111<<offset2)|(~bomb1[10])<<offset2);
              ssd1306_send_byte((((~bomb1[4])>>offset)^B11111111<<offset2)|(~bomb1[11])<<offset2);
              ssd1306_send_byte((((~bomb1[5])>>offset)^B11111111<<offset2)|(~bomb1[12])<<offset2);
              ssd1306_send_byte((((~bomb1[6])>>offset)^B11111111<<offset2)|(~bomb1[13])<<offset2);
              ssd1306_send_data_stop();      
       }
       if (bombY[tb]>=16){
              ssd1306_setpos(bombX[tb],bombY[tb]/8-2);
              ssd1306_send_data_start();
              ssd1306_send_byte((~bomb1[0])<<offset2);
              ssd1306_send_byte((~bomb1[1])<<offset2);
              ssd1306_send_byte((~bomb1[2])<<offset2);
              ssd1306_send_byte((~bomb1[3])<<offset2);
              ssd1306_send_byte((~bomb1[4])<<offset2);
              ssd1306_send_byte((~bomb1[5])<<offset2);
              ssd1306_send_byte((~bomb1[6])<<offset2);
              ssd1306_send_data_stop();
       }
       if (bombY[tb]>56 && (playerOffset<bombX[tb]+7&&playerOffset+5>bombX[tb])){
         
         stopAnimate=1;
         
       }
       
       if (bombY[tb]>59){
              analogWrite(1,127);
              ssd1306_setpos(bombX[tb],bombY[tb]/8-1);
              ssd1306_send_data_start();
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_data_stop();
         
              ssd1306_setpos(bombX[tb],7);
              ssd1306_send_data_start();
              ssd1306_send_byte(B00111100);
              ssd1306_send_byte(B01011010);
              ssd1306_send_byte(B10100101);
              ssd1306_send_byte(B01010101);
              ssd1306_send_byte(B10101010);
              ssd1306_send_byte(B01011010);
              ssd1306_send_byte(B00111100);
              ssd1306_send_data_stop();
       }
       if (bombY[tb]==62){
              ssd1306_setpos(bombX[tb],bombY[tb]/8);
              ssd1306_send_data_start();
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_byte(B00000000);
              ssd1306_send_data_stop();
              
              bombStatus[tb] = 0;
              score++;
              digitalWrite(1,0);
       }
       
    }else {
      
    }
  }
  //delay(1000);
}
void drawPlayer(){
  ssd1306_setpos(playerOffset-5,7);
  ssd1306_send_data_start();
  ssd1306_send_byte(B00000000); // erase player's previous position
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_byte(B00000000);
  ssd1306_send_data_stop();
  if (playerStatus==1&&quadFrameCount<2){
    ssd1306_draw_bmp(playerOffset,7,playerOffset+5,8,playerRight1);
  }else if (playerStatus==1){
    ssd1306_draw_bmp(playerOffset,7,playerOffset+5,8,playerRight2);
  }else if (playerStatus==2&&quadFrameCount<2){
    ssd1306_draw_bmp(playerOffset,7,playerOffset+5,8,playerLeft1);
  }else if (playerStatus==2){
    ssd1306_draw_bmp(playerOffset,7,playerOffset+5,8,playerLeft2);
  }else if (quadFrameCount<2){
    ssd1306_draw_bmp(playerOffset,7,playerOffset+5,8,playerIdle1);
  }else {
    ssd1306_draw_bmp(playerOffset,7,playerOffset+5,8,playerIdle2); 
  }
}

void beep(int bCount,int bDelay){
  for (int i = 0; i<=bCount; i++){digitalWrite(1,HIGH);for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}digitalWrite(1,LOW);for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}}
}

void ssd1306_init(void){
	DDRB |= (1 << SSD1306_SDA);	// Set port as output
	DDRB |= (1 << SSD1306_SCL);	// Set port as output

	ssd1306_send_command(0xAE); // display off
	ssd1306_send_command(0x00); // Set Memory Addressing Mode
	ssd1306_send_command(0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_send_command(0x40); // Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_send_command(0x81); // Set COM Output Scan Direction
	ssd1306_send_command(0xCF); // ---set low column address
	ssd1306_send_command(0xA1); // ---set high column address
	ssd1306_send_command(0xC8); // --set start line address
	ssd1306_send_command(0xA6); // --set contrast control register
	ssd1306_send_command(0xA8);
	ssd1306_send_command(0x3F); // --set segment re-map 0 to 127
	ssd1306_send_command(0xD3); // --set normal display
	ssd1306_send_command(0x00); // --set multiplex ratio(1 to 64)
	ssd1306_send_command(0xD5); // 
	ssd1306_send_command(0x80); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_send_command(0xD9); // -set display offset
	ssd1306_send_command(0xF1); // -not offset
	ssd1306_send_command(0xDA); // --set display clock divide ratio/oscillator frequency
	ssd1306_send_command(0x12); // --set divide ratio
	ssd1306_send_command(0xDB); // --set pre-charge period
	ssd1306_send_command(0x40); // 
	ssd1306_send_command(0x20); // --set com pins hardware configuration
	ssd1306_send_command(0x02);
	ssd1306_send_command(0x8D); // --set vcomh
	ssd1306_send_command(0x14); // 0x20,0.77xVcc
	ssd1306_send_command(0xA4); // --set DC-DC enable
	ssd1306_send_command(0xA6); // 
	ssd1306_send_command(0xAF); // --turn on oled panel 
}

void ssd1306_xfer_start(void){
	DIGITAL_WRITE_HIGH(SSD1306_SCL);	// Set to HIGH
	DIGITAL_WRITE_HIGH(SSD1306_SDA);	// Set to HIGH
	DIGITAL_WRITE_LOW(SSD1306_SDA);		// Set to LOW
	DIGITAL_WRITE_LOW(SSD1306_SCL);		// Set to LOW
}

void ssd1306_xfer_stop(void){
	DIGITAL_WRITE_LOW(SSD1306_SCL);		// Set to LOW
	DIGITAL_WRITE_LOW(SSD1306_SDA);		// Set to LOW
	DIGITAL_WRITE_HIGH(SSD1306_SCL);	// Set to HIGH
	DIGITAL_WRITE_HIGH(SSD1306_SDA);	// Set to HIGH
}

void ssd1306_send_byte(uint8_t byte){
	uint8_t i;
	for(i=0; i<8; i++)
	{
		if((byte << i) & 0x80)
			DIGITAL_WRITE_HIGH(SSD1306_SDA);
		else
			DIGITAL_WRITE_LOW(SSD1306_SDA);
		
		DIGITAL_WRITE_HIGH(SSD1306_SCL);
		DIGITAL_WRITE_LOW(SSD1306_SCL);
	}
	DIGITAL_WRITE_HIGH(SSD1306_SDA);
	DIGITAL_WRITE_HIGH(SSD1306_SCL);
	DIGITAL_WRITE_LOW(SSD1306_SCL);
}

void ssd1306_send_command(uint8_t command){
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);  // Slave address, SA0=0
	ssd1306_send_byte(0x00);	// write command
	ssd1306_send_byte(command);
	ssd1306_xfer_stop();
}

void ssd1306_send_data_start(void){
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);
	ssd1306_send_byte(0x40);	//write data
}

void ssd1306_send_data_stop(void){
	ssd1306_xfer_stop();
}

void ssd1306_setpos(uint8_t x, uint8_t y)
{
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);  //Slave address,SA0=0
	ssd1306_send_byte(0x00);	//write command

	ssd1306_send_byte(0xb0+y);
	ssd1306_send_byte(((x&0xf0)>>4)|0x10); // |0x10
	ssd1306_send_byte((x&0x0f)|0x01); // |0x01

	ssd1306_xfer_stop();
}

void ssd1306_fillscreen(uint8_t fill_Data){
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		ssd1306_send_command(0xb0+m);	//page0-page1
		ssd1306_send_command(0x00);		//low column start address
		ssd1306_send_command(0x10);		//high column start address
		ssd1306_send_data_start();
		for(n=0;n<128;n++)
		{
			ssd1306_send_byte(fill_Data);
		}
		ssd1306_send_data_stop();
	}
}

void ssd1306_char_f6x8(uint8_t x, uint8_t y, const char ch[]){
	uint8_t c,i,j=0;
	while(ch[j] != '\0')
	{
		c = ch[j] - 32;
		if(x>126)
		{
			x=0;
			y++;
		}
		ssd1306_setpos(x,y);
		ssd1306_send_data_start();
		for(i=0;i<6;i++)
		{
			ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font6x8[c*6+i]));
		}
		ssd1306_send_data_stop();
		x += 6;
		j++;
	}
}


void system_sleep() {
  ssd1306_fillscreen(0x00);
  digitalWrite(1,LOW);
  ssd1306_send_command(0xAE);
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON  
  ssd1306_send_command(0xAF);
  activeMillis = millis();
   ssd1306_init();
   digitalWrite(1,HIGH);
}

void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
{
	uint16_t j = 0;
	uint8_t y;
	if (y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		ssd1306_setpos(x0,y);
		ssd1306_send_data_start();
		for (uint8_t x = x0; x < x1; x++)
		{
			ssd1306_send_byte(~pgm_read_byte(&bitmap[j++]));
		}
		ssd1306_send_data_stop();
	}
}
