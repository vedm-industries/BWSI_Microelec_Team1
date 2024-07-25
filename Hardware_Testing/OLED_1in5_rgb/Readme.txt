

Below are all of the connections that need to be make with the Arduino

Arduino
	CS	10
	DC	7
	RST	8
SPI:	
	DIN	11	
	CLK	13

IMPORTANT(ish) NOTES!!
- Baud is set by System_Init(); at the beginning of the sketch, so make sure your serial monitor is at 115200
- Paint_DrawString_EN(); is a weird func it doesn't take allow you to put in strings, so I just made a variable
	cPtr that pointed to a character array (which I updated by iterating over it). Is it the best solution? 
	No. If someone understands cpp better pls fix the code in OLED_attempt_1.ino ;-;
- For some reason you can't have two .ino files in the folder when you run it, so if you have multiple, just 
	comment out one file entirely
	
