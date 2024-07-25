#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"

char message[20] = "Hello, World!"; // Allocate enough space for "Hello, World!" and modifications
char* cPtr = message; // Pointer to the message array

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
    Serial.println("Drawing: page 3");     
    Paint_DrawString_EN(10, 0, cPtr, &Font16, BLACK, BLUE);
    Paint_DrawNum(10, 30, "123.4567", &Font12, 2, RED, BLACK); 
    Paint_DrawString_CN(10, 50, "你好Ab", &Font12CN, BLACK, BROWN);
    Paint_DrawString_CN(0, 80, "微雪电子", &Font24CN, BLACK, BRED);
    Driver_Delay_ms(2000);    
    OLED_1in5_rgb_Clear(); 
    Serial.println("WHAT");  

    // Modify the string
    cPtr[7] = 'K';

    // Copy new string into cPtr, ensuring not to exceed the array bounds
    char newMessage[] = "yooo";
    int i;
    for (i = 0; i < sizeof(newMessage); i++) {
        cPtr[i] = newMessage[i];
    }
    // Ensure null-termination
    cPtr[i] = '\0';
    Serial.println("main loop");
    delay(2000); // Add delay for readability
}
