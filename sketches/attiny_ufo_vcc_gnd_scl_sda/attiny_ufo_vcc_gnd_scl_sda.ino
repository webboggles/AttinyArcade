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




boolean stopAnimate = 0; // this is set to 1 when a collision is detected
byte maxObstacles = 1; // this defines the max number of in game obstacles
byte obstacleStep = 2; // pixel step of obstacles per frame
int obstacle[9] = {-50,-50,-50,-50,-50,-50,-50,-50,-50}; // x offset of the obstacle default position, out of view
byte gapOffset[9] = {0,0,0,0,0,0,0,0,0}; // y offset of the fly-through gap
int gapSize[9]; // y height of the gap
byte maxGap = 60; // max height of the gap
int stepsSinceLastObstacle = 0; // so obstacles are not too close
byte gapBlock[9] = {0,0,0,0,0,0,0,0,0}; // if the fly-through gap is closed
byte blockChance = 0; // this higher value decreases the likelihood of gap being closed 
boolean fire = 0; // set to 1 when the fire interrupt is triggered
byte fireCount = 0; // the shot is persistent for several frames
byte playerOffset = 0; // y offset of the top of the player
byte flames = 0; // this is set to 1 when the move up interrupt is triggered

byte flameMask[2]={B00111111,B11111111}; // this is used to only show the flame part of the icon when moving up


int score = 0; // score - this affects the difficulty of the game
ISR(PCINT0_vect){ // PB0 pin button interrupt			     
   //if (playerOffset >1&&stopAnimate==0){playerOffset-=1;} // for debounce, the movement is in the main loop//
   return;
}
void playerInc(){ // PB2 pin button interrupt
   fire = 1;
   fireCount = 5; // number of frames the shot will persist
}

void setup() {
  DDRB = 0b00000010;  	// set PB1 as output (for the speaker)
  PCMSK = 0b00000001;	// pin change mask: listen to portb bit 1
  GIMSK |= 0b00100000;	// enable PCINT interrupt 
  sei();	        // enable all interrupts
  attachInterrupt(0,playerInc,RISING);
  
  
  resetGame();
}
void loop() { 

      noInterrupts();
      
              
              //update game vars to make it harder to play
              if (score < 500){blockChance = 11-score/50; if (maxObstacles<5){maxObstacles=(score+40)/70+1;} delayMicroseconds(16000/maxObstacles);}

              if (score < 2000){maxGap = 60-score/100;}
              if (fire == 1){score--;}
              if (fireCount>0){fireCount--;}
              
                if (digitalRead(0)==1){if (playerOffset >0 && stopAnimate==0){playerOffset--; flames = 1; // move player up
                  for (int i = 0; i<2; i++){
                    beep(1,random(0,i*2));
                  }
                }} 
                if (digitalRead(0)==1){if (playerOffset >0 && stopAnimate==0){playerOffset--; flames = 1; // move player up
                  for (int i = 0; i<2; i++){
                    beep(1,random(0,i*2));
                  }
                }}
                if (digitalRead(0)==1){if (playerOffset >0 && stopAnimate==0){playerOffset--; flames = 1; // move player up
                  for (int i = 0; i<2; i++){
                    beep(1,random(0,i*2));
                  }
                }}
                stepsSinceLastObstacle += obstacleStep;
                for (byte i = 0; i<maxObstacles;i++){ // fly obstacles
                  if (obstacle[i] >= 0 && obstacle[i] <= 128 && stopAnimate==0){
                    obstacle[i] -= obstacleStep;
                    if (gapBlock[i]>0 && obstacle[i] < 36  && playerOffset>gapOffset[i] && playerOffset+5<gapOffset[i]+gapSize[i] && fireCount > 0){//
                       gapBlock[i] = 0;
                       score += 5; 
                       for (byte cp = 400; cp>0; cp--){
                         beep(1,cp);
                       }
                    }
                  } 
                  
                  if (obstacle[i]<=4 && stepsSinceLastObstacle>=random(30,100)){ // generate new obstacles
                    obstacle[i] = 123;
                    gapSize[i] = random(25,maxGap);
                    gapOffset[i] = random(0,64-gapSize[i]);
                    if (random(0,blockChance)==0){gapBlock[i] = 1;}else {gapBlock[i] = 0;}
                    stepsSinceLastObstacle = 0;
                    score+=1;
                  }
                }
                
                if (playerOffset < 56 && stopAnimate==0){playerOffset++;} // player gravity
                

               // if (stopAnimate==0){ssd1306_clearscreen();}
              
    
              // update whats on the screen
                 
                  
                  // erase player
                 
                 for (byte r=0; r<8; r++){
                   if (r<playerOffset/8 | r >= playerOffset/8+1){
                 ssd1306_setpos(0,r);
                 ssd1306_send_data_start();
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
                 ssd1306_send_byte(B00000000);
                 ssd1306_send_byte(B00000000);
                 
                 ssd1306_send_data_stop();
                   }
                 }
                 
                 //erase fire
                 ssd1306_setpos(16,playerOffset/8);
                 ssd1306_send_data_start();
                 if (fireCount == 0){
                                for (byte f = 0; f<=28; f++){
                                    ssd1306_send_byte(B00000000);
                                }
                 }
                 ssd1306_send_data_stop();
                 
                 
                  // Send Obstacle
                  for (byte i = 0; i<maxObstacles;i++){
                    if (obstacle[i] >= -5 && obstacle[i] <= 128){ // only deal with visible obstacles
                      if (obstacle[i] > 8 && obstacle[i] <16){ // look for collision if obstacle is near the player
                        if (playerOffset < gapOffset[i] || playerOffset+5 > gapOffset[i]+gapSize[i] || gapBlock[i] != 0){
                          // collision!
                          stopAnimate = 1; 
                          // process collision after drawing explosion
                        }
                      }                      
                      
                      for (byte row = 0; row <8; row++){
                        
                          ssd1306_setpos(obstacle[i],row);
                          ssd1306_send_data_start();
                          
                          if (obstacle[i]>0&&obstacle[i] < 128){
                             
                             if ((row+1)*8 - gapOffset[i] <= 8){ // generate obstacle : top and transition
                                byte temp = B11111111>>((row+1)*8 - gapOffset[i]); 
                                byte tempB = B00000000; 
                                if (gapBlock[i]>0){tempB=B10101010;}
                                ssd1306_send_byte(temp);
                                ssd1306_send_byte(temp|tempB>>1);
                                ssd1306_send_byte(temp|tempB);
                                ssd1306_send_byte(temp);
                                
                             }else if (row*8>=gapOffset[i] && (row+1)*8<=gapOffset[i]+gapSize[i]){ // middle gap
                                byte tempB = B00000000; 
                                if (gapBlock[i]>0){tempB=B10101010;}
                                ssd1306_send_byte(B00000000);
                                ssd1306_send_byte(B00000000|tempB>>1);
                                ssd1306_send_byte(B00000000|tempB);
                                ssd1306_send_byte(B00000000);

                             }else if ((gapOffset[i] +gapSize[i]) >= row*8 && (gapOffset[i] +gapSize[i]) <= (row+1)*8){ // bottom transition
                                //}else if ((gapOffset[i] +gapSize[i]) >= row*8 && (gapOffset[i] +gapSize[i]) <= (row+1)*8){ // bottom transition
                                //byte temp = B11111111<<((gapOffset[i] + gapSize[i])%8); 
                                
                                byte temp = B11111111<<((gapOffset[i] + gapSize[i])%8); 
                                byte tempB = B00000000; 
                                if (gapBlock[i]>0){tempB=B10101010;}
                                ssd1306_send_byte(temp);
                                ssd1306_send_byte(temp|tempB>>1);
                                ssd1306_send_byte(temp|tempB);
                                ssd1306_send_byte(temp);
                                
                             }else { // fill rest of obstacle
                                ssd1306_send_byte(B11111111);
                                ssd1306_send_byte(B11111111);
                                ssd1306_send_byte(B11111111);
                                ssd1306_send_byte(B11111111);
                             }
                             
                             ssd1306_send_byte(B00000000);
                             ssd1306_send_byte(B00000000);
                             
                          ssd1306_send_data_stop();
                          }
                        }
                   
                    }
                  }
                  
                 

                 if (playerOffset%8!=0){ // overflow the player icon into the next screen row if split
                      ssd1306_setpos(8,playerOffset/8);
                      ssd1306_send_data_start();
                        if (stopAnimate==0){
                            ssd1306_send_byte((B00001100&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames])<<playerOffset%8);
                            ssd1306_send_byte((B00001100&flameMask[flames])<<playerOffset%8);
                            if (fireCount >0){
                                for (byte f = 0; f<=24; f++){
                                    ssd1306_send_byte(B00000100<<playerOffset%8);
                                }
                                ssd1306_send_byte(B00010101<<playerOffset%8);
                                ssd1306_send_byte(B00001010<<playerOffset%8);
                                ssd1306_send_byte(B00010101<<playerOffset%8);
                                if (fire==1){beep(50,100);}
                                fire = 0;
                              
                            }
                        }else {
                            ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))<<playerOffset%8);
                            ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))<<playerOffset%8);
                        }
                        
                      ssd1306_send_data_stop();
                      ssd1306_setpos(8,playerOffset/8+1);
                      ssd1306_send_data_start();
                        if (stopAnimate==0){
                            ssd1306_send_byte((B00001100&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames])>>8-playerOffset%8);
                            ssd1306_send_byte((B00001100&flameMask[flames])>>8-playerOffset%8);
                            if (fireCount >0){
                                for (byte f = 0; f<=24; f++){
                                    ssd1306_send_byte(B00000100>>8-playerOffset%8);
                                }
                                ssd1306_send_byte(B00010101>>8-playerOffset%8);
                                ssd1306_send_byte(B00001010>>8-playerOffset%8);
                                ssd1306_send_byte(B00010101>>8-playerOffset%8);
                                if (fire==1){beep(50,100);}
                                fire = 0;
                            }
                        }else {
                            ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                            ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))>>8-playerOffset%8);
                        }
                      ssd1306_send_data_stop();
                      }else {
                          ssd1306_setpos(8,playerOffset/8);
                          ssd1306_send_data_start();
                          if (stopAnimate == 0){
                            ssd1306_send_byte(B00001100&flameMask[flames]);
                            ssd1306_send_byte(B01011110&flameMask[flames]);
                            ssd1306_send_byte(B10010111&flameMask[flames]);
                            ssd1306_send_byte(B01010011&flameMask[flames]);
                            ssd1306_send_byte(B01010011&flameMask[flames]);
                            ssd1306_send_byte(B10010111&flameMask[flames]);
                            ssd1306_send_byte(B01011110&flameMask[flames]);
                            ssd1306_send_byte(B00001100&flameMask[flames]);
                            if (fireCount >0){
                                for (byte f = 0; f<=24; f++){
                                    ssd1306_send_byte(B00000100);
                                }
                                ssd1306_send_byte(B00010101);
                                ssd1306_send_byte(B00001010);
                                ssd1306_send_byte(B00010101);
                                if (fire==1){beep(50,100);}
                                fire = 0;
                            }
                          }else {
                            ssd1306_send_byte(B00001100&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B01011110&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B10010111&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B01010011&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B01010011&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B10010111&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B01011110&flameMask[flames] | random(0,255));
                            ssd1306_send_byte(B00001100&flameMask[flames] | random(0,255)); 
                          }
                          ssd1306_send_data_stop();
                      }
                      
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
                          interrupts();
                          system_sleep();
                          resetGame();
                          noInterrupts();  
                      }
                      //ssd1306_char_f6x8(32, 8, "score:");
                      
                      char temp[10] = {0,0,0,0,0,0,0,0,0,0};
                      itoa(score,temp,10);
                      ssd1306_char_f6x8(92, 0, temp);
                          
                          
                      flames = 0;
          	  interrupts();
                  
             //    
                     
              
              
              
              if (stopAnimate == 1){
                  for (int i = 0; i<1000; i++){
                    beep(1,random(0,i*2));
                  }
                 delay(2000);
                 system_sleep(); 
              }
  
}
void resetGame(){
  ssd1306_init();
  
  for (byte s = 0; s<30; s++){ // generate stars
     ssd1306_setpos(random(0,127),random(0,7));
     ssd1306_send_data_start();
     ssd1306_send_byte(B10000000>>random(0,7)); 
     ssd1306_send_data_stop();
     beep(10+s,600);
     
  }
  ssd1306_char_f6x8(8, 2, "U F O  E S C A P E");
  delay(200);
  ssd1306_char_f6x8(18, 4, "webboggles.com");
  delay(200);
  ssd1306_char_f6x8(12, 6, "Tweet @webboggles");  
    ssd1306_char_f6x8(22, 7, "#AttinyArcade");  
  delay(2000);
  
  
  ssd1306_fillscreen(0x00);
  stopAnimate = 0;
  score = 0;

  maxObstacles = 3;
  obstacleStep = 2;
  for (byte i = 0; i<9; i++){
    obstacle[i] = -50;
    gapOffset[i]=0;
  }
  stepsSinceLastObstacle = 0;
  playerOffset = 0; // y offset of the top of the player
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
}
