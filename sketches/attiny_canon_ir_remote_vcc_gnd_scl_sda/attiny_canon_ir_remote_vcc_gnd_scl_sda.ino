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


long lastInteraction = 0;
long lastShot = 0;
int shotDelay = 0;
int digit1 = 0;
int digit2 = 1;
byte cState = 0;
byte shooting = 0;
boolean flagL = 0;
boolean flagR = 0;

ISR(PCINT0_vect){ // PB0 pin button interrupt			     
   noInterrupts();

   flagL = 1;
  lastInteraction = millis();

   return;
}
void playerInc(){ // PB2 pin button interrupt
      noInterrupts();

      flagR = 1;
    lastInteraction = millis();
    
}

void setup() {
  lastInteraction = millis();
  DDRB = 0b00000010;  	// set PB1 as output (for the speaker)
  PCMSK = 0b00000001;	// pin change mask: listen to portb bit 1
  GIMSK |= 0b00100000;	// enable PCINT interrupt 
  sei();	        // enable all interrupts

  attachInterrupt(0,playerInc,RISING);
  
  resetGame();
  pinMode(1, OUTPUT); 

}
void loop() { 
    
  while (shooting==1 || lastInteraction+15000 > millis()){
      if (shooting == 1 && shotDelay + lastShot < millis()){
        lastShot = millis();
        instantShot(); 
        beep(250,200); delay(50); beep(250,200); delay(50);
      }else if (shooting == 1 && shotDelay + lastShot-1000 < millis()){
        beep(250,500); delay(300);
      }
      if (shooting == 1 && shotDelay + lastShot-300 < millis()){
         ssd1306_fillscreen(0x00);
      }
      if (flagL == 1){ // left button was pressed
          
          
           if (digitalRead(0)){
             beep(150,100); delay(50); beep(250,200); delay(50);
             
             if (cState <5){
               cState++;
             }else {
               cState = 0; 
             }
              
             }
             while (digitalRead(0)){delay(100);}
             eraseSelection();
      }
      if (flagR == 1){ // right button was pressed
        beep(250,100); delay(10); beep(150,200);  beep(50,500); delay(50);
        while (digitalRead(2)){delay(100);}
        switch (cState){
          case 0:
            ssd1306_fillscreen(0x00);
            delay(100);
            instantShot();
          break;
          case 1:
            ssd1306_fillscreen(0x00);
            delay(100);
            delayedShot();
          break;
          case 2:
            if (digit1 <9){
              digit1++;
            }else {
             digit1 = 0; 
            }
            shotDelay = ((digit1*10)+digit2)*1000;
          break;
          case 3:
            if (digit2 <9){
              digit2++;
            }else {
             digit2 = 1; 
            }
            shotDelay = ((digit1*10)+digit2)*1000;
          break;
          case 4:
            lastShot = millis();
            if (shooting == 0){instantShot();}
            shooting = !shooting;
          break;
          case 5:
            cState = 0;
            ssd1306_fillscreen(0x00);
            shooting = 0;
            system_sleep();
            resetGame();
          break;
          default:
          break;
        }
        eraseSelection();
      }
      flagL = 0;
      flagR = 0;
      interrupts();
      ssd1306_char_f6x8(0, 0, "CANON INFRARED REMOTE");
      ssd1306_char_f6x8(0, 2, "Trigger");
      ssd1306_char_f6x8(50, 2, "now    in 2s"); 
      ssd1306_char_f6x8(50, 6, "Exit"); 
      // clear line
      /*ssd1306_setpos(50,3); ssd1306_send_data_start(); for (byte i=0;i<50;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
      ssd1306_setpos(40,4); ssd1306_send_data_start(); for (byte i=0;i<16;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
      ssd1306_setpos(40,5); ssd1306_send_data_start(); for (byte i=0;i<16;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
      ssd1306_setpos(50,7); ssd1306_send_data_start(); for (byte i=0;i<26;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();            
      */
        
      if (shooting == 0){
          ssd1306_char_f6x8(5, 4, "Every");
      }else {
          ssd1306_setpos(0,4); ssd1306_send_data_start(); for (byte i=0;i<128;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
          ssd1306_char_f6x8(0, 4, "Next:");        
      }
      if (shooting == 0){
        ssd1306_char_f6x8(57, 4, "s  START");
      }else {
       ssd1306_char_f6x8(57, 4, "s  STOP "); 
      }
      
      if (shooting == 0){
        char temp[1] = {0};
        itoa(digit1,temp,10);
        ssd1306_char_f6x8(40, 4, temp);
        
        itoa(digit2,temp,10);
        ssd1306_char_f6x8(48, 4, temp);
      }
      if (shooting == 1 ){
        char temp2[5] = {0,0,0,0,0};
        itoa(constrain((lastShot+shotDelay-millis())/1000,0,shotDelay/1000),temp2,10);
        ssd1306_char_f6x8(40, 4, temp2);
      }

      
      switch (cState){ // underline option
        case 0: // instant shot
            ssd1306_setpos(50,3);
            ssd1306_send_data_start();
            for (byte i=0;i<19;i++){ssd1306_send_byte(B00000011);}
            
            
            ssd1306_send_data_stop();
            break;
        case 1: // delayed shot
            ssd1306_setpos(94,3);
            ssd1306_send_data_start();
            for (byte i=0;i<28;i++){ssd1306_send_byte(B00000011);}
            ssd1306_send_data_stop();
            break;
        case 2:// timelapse digit1
            ssd1306_setpos(40,5);
            ssd1306_send_data_start();
            for (byte i=0;i<7;i++){ssd1306_send_byte(B00000011);}
            ssd1306_send_data_stop();
            break;
        case 3: // timelapse digit2
            ssd1306_setpos(48,5);
            ssd1306_send_data_start();
            for (byte i=0;i<7;i++){ssd1306_send_byte(B00000011);}
            ssd1306_send_data_stop();
            break;
         case 4: // timelapse start/stop
            ssd1306_setpos(75,5);
            ssd1306_send_data_start();
            for (byte i=0;i<25+(shooting==0?6:0);i++){ssd1306_send_byte(B00000011);} // adjust underline to start/stop length
            ssd1306_send_data_stop();
            break;
         case 5: // exit
            ssd1306_setpos(50,7);
            ssd1306_send_data_start();
            for (byte i=0;i<25;i++){ssd1306_send_byte(B00000011);}
            ssd1306_send_data_stop();
            break;
         default:
         break;
      }
  }    

     
          ssd1306_fillscreen(0x00);
          system_sleep();
          resetGame();
  
     
  
}

void eraseSelection(){
  ssd1306_setpos(0,1); ssd1306_send_data_start(); for (byte i=0;i<128;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
  ssd1306_setpos(0,3); ssd1306_send_data_start(); for (byte i=0;i<128;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
  ssd1306_setpos(0,5); ssd1306_send_data_start(); for (byte i=0;i<128;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
  ssd1306_setpos(0,7); ssd1306_send_data_start(); for (byte i=0;i<128;i++){ssd1306_send_byte(B00000000);} ssd1306_send_data_stop();
}
void instantShot(){
  for(int i=0; i<16; i++) { 
    PORTB |= _BV(PORTB1);
    delayMicroseconds(16);
    PORTB &= ~_BV(PORTB1);
    delayMicroseconds(16);
   } 
   delay(7); 
   for(int i=0; i<16; i++) { 
     PORTB |= _BV(PORTB1);
     delayMicroseconds(16);
     PORTB &= ~_BV(PORTB1);
     delayMicroseconds(16);
   }  
}
void delayedShot(){
  for(int i=0; i<16; i++) { 
    PORTB |= _BV(PORTB1);
    delayMicroseconds(16);
    PORTB &= ~_BV(PORTB1);
    delayMicroseconds(16);
   } 
   delayMicroseconds(5360); 
   for(int i=0; i<16; i++) { 
     PORTB |= _BV(PORTB1);
     delayMicroseconds(16);
     PORTB &= ~_BV(PORTB1);
     delayMicroseconds(16);
   }  
}

void resetGame(){
  ssd1306_init();
  delay(100);
  ssd1306_char_f6x8(0, 2, "CANON INFRARED REMOTE");
  delay(200);
  ssd1306_char_f6x8(10, 4, "by webboggles.com");

  beep(150,100);
  delay(50);
  
  beep(250,200);
  delay(50);
  
  beep(140,500);
  delay(50);
  
 
  
  
  
  
  delay(2000);
  
  
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
  

  ssd1306_init();


}
