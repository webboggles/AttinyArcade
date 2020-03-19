# AttinyArcade
Attiny85 based Arduino compatible keychain game platform
================================================================
UPDATE 19 MARCH 2020:
Added sketch for temperature monitor with Dallas TO92 sensor. 4.6K resistor required between V and DATA.

------------------------------------------

Project page: http://webboggles.com/attiny85-game-kit-assembly-instructions/
Community photos and updates: https://twitter.com/hashtag/AttinyArcade

SSD1306 library courtesy of Tinusaur: https://bitbucket.org/tinusaur/ssd1306xled

Video instructions: https://youtu.be/iT9dfrffQh4

If you have problems uploading large sketches — you need to update ld.exe in arduino\hardware\tools\avr\avr\bin
https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC


Assembly
------------------------------------------
See documentation folder for DIY pcb, schematic and pdf instructions.

Parts list
- 3D Printed Snap Case
- Printed Circuit Board
- SSD1306 OLED Screen
- 4 pin 2.54 female header
- CR2032 Battery
- CR2032 Battery Holder
- Pulldown resistor x2
- Pushbutton x2
- Attiny85
- DIP8 socket
- Piezo speaker
- 10uf capacitor (used with arduino when programming Attiny85) 	


Games and sketches developed by Ilya Titov https://twitter.com/webboggles
------------------------------------------
- Breakout (2014) — Original blog post: http://webboggles.com/attiny85-breakout-keychain-game/
- UFO Escape (2015) — Original blog post: http://webboggles.com/attiny85-ufo-escape-keychain-game/
- Canon DSLR IR Remote (2016) — Just replace the piezo with an IR LED
- Oroboros (2016) — Inspired by Snake
- Run Dude, Run! (2017) — Don't get hit by those bombs!


Games developed by Andy Jackson https://twitter.com/andyhighnumber
------------------------------------------
Visit Andy's repo and google drive for more info and sketches:
https://github.com/andyhighnumber/Attiny-Arduino-Games
https://drive.google.com/drive/folders/0BxGfuaPdM4n0Nkt5TDI4azd1Rmc

- Wren Rollercoaster
- Tetris
- Space Attack
- UFO + Stacker
- UFO + Breakout
- Bat Bonanza
- Frogger


Other contributors
------------------------------------------
Breakout, the first game was created after much inspiration from the Tinusaur project: http://tinusaur.org/.
The screen control and font functions written by Neven Boyanov for the http://tinusaur.wordpress.com/ project
Source code and font files available at: https://bitbucket.org/tinusaur/ssd1306xled

Sleep code is based on this blog post by Matthew Little:
http://www.re-innovation.co.uk/web12/index.php/en/blog-75/306-sleep-modes-on-attiny85
 
Alvin Starkey created great wooden variations of the game and contributed to the development. 
For more info visit his twitter feed @AlvinStar10.

@senkunmusashi Contributed to the creation of the Frogger game.



