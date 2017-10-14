/* 2015
 * UFO Escape game by Ilya Titov. Find building instructions on http://webboggles.com/
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


byte len = 3;
int maxLen = 100;
byte dir = 2; // 1 up 2 right 3 bottom 4 left
byte nextDir = 1;
unsigned long screenBuffer[16];
boolean stopAnimate = 0; // 
byte xPos[100];
byte yPos[100];
boolean dirPlus = 0;
boolean dirMinus = 0;
unsigned long lastFrame = 0;
int frameDelay = 300;
boolean selfCollision = 0;
int baitX = 0;
int baitY = 0;
boolean baitDropped = 0;
long timeout = 10000;
boolean sleeping = 0;
unsigned long nextObstacle = 0;

int score = 0; // score - this affects the difficulty of the game
ISR(PCINT0_vect){ // PB0 pin button interrupt			     
   //if (playerOffset >1&&stopAnimate==0){playerOffset-=1;} // for debounce, the movement is in the main loop//
   delay(100);
   if (digitalRead(0)){
   //  while(digitalRead(0)){
       dirMinus = 1;
       dirPlus = 0;
     }
   //}
  noInterrupts();
   return;
}
void playerInc(){ // PB2 pin button interrupt
      delay(100);
//    while(digitalRead(2)){
      dirPlus = 1;
      dirMinus = 0;
//    }
    noInterrupts();
}

void setup() {

  DDRB = 0b00000010;  	// set PB1 as output (for the speaker)
  PCMSK = 0b00000001;	// pin change mask: listen to portb bit 1
  GIMSK |= 0b00100000;	// enable PCINT interrupt 
  sei();	        // enable all interrupts
  attachInterrupt(0,playerInc,RISING);
  
  // player origin
  xPos[2] = 14;
  yPos[2] = 7;
  xPos[1] = 15;
  yPos[1] = 7;
  xPos[0] = 16;
  yPos[0] = 7;

  
  timeout = millis()+40000;
  
  //for (int i = 0; i<8; i++){
    //screenBuffer[i*2] = ((((((0UL | B10101010) << 8) | B10101010) << 8) | B10101010) << 8  | B10101010);
    //screenBuffer[i*2+1] = B01010101;
  //}
  //screenBuffer[15] = ((((((0UL | B11111111) << 8) | B11111111) << 8) | B11111111) << 8  | B11111111);
  

  
  resetGame();
  

}
void loop() { 
    
  while (1==1){

    
  
      
      if (millis()>lastFrame+frameDelay && stopAnimate == 0){
        lastFrame = millis();
        beep(50,200);
        if (score<100){frameDelay  = 300 - score*3 ;}
        for (byte uro = len-1; uro > 0; uro--){ //move uro
          xPos[uro] =xPos[uro-1];
          yPos[uro] =yPos[uro-1];
        }
        if (dirMinus==1 && sleeping==0){
          if (dir>0){dir--;}else {dir = 3;}
         dirMinus = 0;
         timeout = millis()+40000;
        }
        if (dirPlus==1 && sleeping==0){
         if (dir<3){dir++;}else {dir = 0;}
          dirPlus = 0;
          timeout = millis()+40000;
        }
        

        sleeping = 0;
        if (timeout < millis()){
            sleeping = 1;
            system_sleep();
            
        }

        
        switch (dir){
          case 0:
            if (yPos[0]>0){
              yPos[0]--;
            }else {
              yPos[0]=15; 
            }
          break;
          case 1:
            if (xPos[0]<31){
              xPos[0]++;
            }else {
              xPos[0]=1; 
            }
          break;
          case 2:
            if (yPos[0]<15){
              yPos[0]++;
            }else {
              yPos[0]=0; 
            }
          break;
          case 3:
            if (xPos[0]>1){
                xPos[0]--;
              }else {
                xPos[0]=31; 
              }
          break;
          default:
          break;
        }
        
        
               
        
        
        
        // CLEAR SCREEN BUFFER
        for (int i = 0; i<16; i++){
          screenBuffer[i] = 0;
        }
        
        
         
        
        
        // Drop bait
        if (1==1){
          while (baitDropped == 0){
            baitX = random(1,30);
            baitY = random(1,14);
            boolean flag = 1;
            for (int uro = len-1; uro >= 0; uro--){
              if (yPos[uro]==baitY && xPos[uro]==baitX){flag = 0;}
            }
            if (flag == 1){baitDropped = 1;}
          }
          
        } 
        
        if (yPos[0] == baitY && xPos[0] == baitX){

          beep(150,100);
          delay(50);
          beep(50,150);
          delay(50);
          beep(50,150);
          delay(50);
          beep(100,150);
          delay(50);
          
          baitDropped = 0;
          baitX = 35;
          baitY = 35;
          score++;
          len++;
          if (len > maxLen){len = maxLen;}

        }
        
        if (baitDropped==1 ){screenBuffer[baitY] = screenBuffer[baitY] | ((0UL | B00000001) << (31-baitX));}
        
        for (int uro = len-1; uro >= 0; uro--){ //add uro to screen buffer
          screenBuffer[yPos[uro]] = screenBuffer[yPos[uro]] | ((0UL | B00000001) << (31-xPos[uro]));
          if (yPos[uro]==yPos[0] && xPos[uro] == xPos[0] && uro != 0){
            //Self collision
            selfCollision = 1;
            stopAnimate = 1;
          }
        }
        // RENDER THE 32x16 GAME GRID
        if (nextObstacle < millis()){
          for (byte r = 0; r<8; r++){
            ssd1306_setpos(0,r);
            ssd1306_send_data_start();
            for (byte col = 1; col<=31; col++){
             
                for (byte box = 0; box < 4; box++){
                  
                    
                  
                  
                  ssd1306_send_byte( // draw screen buffer data and screen boundaries 
                                     // first byte is top //   && dir == 0 ? B00001000
                    (screenBuffer[r*2]>>(31-col)   & B00000001 ? ((box==1||box==2)&&baitX==col&&baitY==r*2&&baitDropped==1?B00001001:B00001111):B00000000) 
                    |
                    (screenBuffer[r*2+1]>>(31-col) & B00000001 ? ((box==1||box==2)&&baitX==col&&baitY==r*2+1&&baitDropped==1?B10010000:B11110000):B00000000)
                    |
                    (r==0?B00000001:B00000000)
                    |
                    (r==7?B10000000:B00000000)
                    |
                    ((col == 1 && box == 0)||(col == 31 && box == 3)?B11111111:B00000000)
                    );
                }
            }
            ssd1306_send_data_stop();
          }
        }
        interrupts();
      }
      // 
      
      
      // display score
      if (stopAnimate==1){
       int topScore = EEPROM.read(0);
          topScore = topScore << 8;
          topScore = topScore |  EEPROM.read(1);
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

      
      

    
  }
}


void resetGame(){
  ssd1306_init();

  ssd1306_char_f6x8(16, 2, "O R O B O R O S");
  delay(200);
  ssd1306_char_f6x8(10, 4, "by webboggles.com");
  delay(200);
  ssd1306_char_f6x8(12, 6, "Tweet @webboggles");  
  ssd1306_char_f6x8(22, 7, "#AttinyArcade"); 

  beep(100,300);
  delay(50);
  
  beep(100,300);
  delay(50);
  
  beep(350,500);
  delay(50);
  
  beep(150,100);
  delay(50);
  
  beep(250,200);
  delay(50);
  
  beep(150,100);
  delay(50);
  beep(250,200);
  delay(50);
    beep(150,100);
  delay(50);
  beep(250,200);
  delay(150);
  beep(150,100);
  delay(50);
  beep(650,150);
  delay(100);
  beep(50,150);
  delay(50);
  beep(50,150);
  delay(50);
  beep(100,150);
  delay(50);
  

  
  delay(2000);
  
  len = 3;
  score = 0;
  stopAnimate = 0; // 
  dirPlus = 0;
  dirMinus = 0;
  frameDelay = 300;
  selfCollision = 0;
  baitDropped = 0;
  
  ssd1306_fillscreen(0x00);


}



void beep(int bCount,int bDelay){
  for (int i = 0; i<=bCount; i++){digitalWrite(1,HIGH);for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}digitalWrite(1,LOW);for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}}
}
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

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void system_sleep() {
  ssd1306_fillscreen(0x00);
  ssd1306_send_command(0xAE);
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON  
  ssd1306_send_command(0xAF);
  
  timeout = millis()+40000;
  if (dir<3){dir++;}else {dir = 0;}
  ssd1306_init();
  dirPlus = 0;
  dirMinus = 0;
}
