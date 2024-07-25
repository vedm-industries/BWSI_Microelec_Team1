#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"

char message[20] = "Hello, World!"; // Message is super long so there are extra character spaces 
char* cPtr = message; // Pointer to the message array

/* 
RANDOM INFORMATION:
So I guess if you wanted to make a line that was able to be updated, you would
have to establish the base text up here first with something like:
  char informationLabel[20] = "BaseLabel:";
  char* informationLabelPtr = informationLabel;

Then later when calling Paint_DrawString_EN() do something like this:
  Paint_DrawString_EN(x position, y position, informationLabelPtr, font, foregroundColor, backgroundColor);

And THEN if you want to update the text call updateText()

slap in updateText("whatever your message is", informationLabelPtr);

I guarantee you this method is over-engineered and there is a simpler way to do it. I just don't know how lol.
*/

// I made and updateText function so its less horrific to change contents of a line
void updateText (String yourMessage, char* yourPtr) {
  int len = yourMessage.length();
  char newMessage[len + 1];
  yourMessage.toCharArray(newMessage, len + 1);
  Serial.println(newMessage);
  int i;
  for (i = 0; i < sizeof(newMessage); i++) {
        yourPtr[i] = newMessage[i];
    }
    yourPtr[i] = '\0';
}

//Setup area 
void setup() {
    Serial.println("Starting program..."); // Print a test message

    System_Init();
    if(USE_IIC) {
        Serial.println("Only USE_SPI_4W, Please revise DEV_config.h !!!");
        return;
    }

    Serial.println(F("OLED_Init()...\r\n"));
    OLED_1in5_rgb_Init();
    Driver_Delay_ms(500); 
    OLED_1in5_rgb_Clear();  

    //1. Create a new image size
    UBYTE *BlackImage;
    Serial.println("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
    Paint_SetScale(65);
}

void loop() {
    //2. Write directly to memory through the GUI 
    Serial.println("Drawing: page 1");     
    Paint_DrawString_EN(10, 0, cPtr, &Font16, BLACK, BLUE);
    Paint_DrawNum(10, 30, "123.4567", &Font12, 2, RED, BLACK); 
    Paint_DrawString_CN(10, 50, "你好Ab", &Font12CN, BLACK, BROWN);
    Paint_DrawString_CN(0, 80, "微雪电子", &Font24CN, BLACK, BRED);
    Driver_Delay_ms(50);  
    delay(2000); // Add delay for readability  
    OLED_1in5_rgb_Clear(); 
  
    Serial.println("main loop");
    updateText("Goodbye, world :O", cPtr);
    
}
