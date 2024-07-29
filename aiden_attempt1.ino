#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"
#include "PMS.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#include "SoftwareSerial.h"
#include "Air_Quality_Sensor.h"
#define SEALEVELPRESSURE_HPA (1013.25)
SoftwareSerial pmsSerial(2,3);
Adafruit_BME280 bme; // I2C
char message[20] = ""; // Message is super long so there are extra character spaces 
char* cPtr = message; // Pointer to the message array
const int ledPin = 9;
const int taiPin = A0;
const int MQPin = A1;
float MQReading;
float taiReading;
float oneum;
float twofiveum;
float tenum;
float humid;
float AQ;
bool status;
AirQualitySensor sensor(A2);
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
  int i;
  for (i = 0; i < sizeof(newMessage); i++) {
        yourPtr[i] = newMessage[i];
    }
    yourPtr[i] = '\0';
}

//Setup area 
void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600);
  Serial.println("Starting program..."); // Print a test message
  System_Init();

  OLED_1in5_rgb_Init();
  Driver_Delay_ms(500); 
  OLED_1in5_rgb_Clear();  

  //1. Create a new image size
  UBYTE *BlackImage;
  Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
  Paint_SetScale(65);
  status = bme.begin(0x76); 
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms5003data data;

void loop() {
  MQReading = analogRead(MQPin)*5.0/1024;
  taiReading = analogRead(taiPin)*5.0/1024;
  humid = bme.readHumidity();
  AQ = sensor.getValue();
  updateText("Taicident:" + String(taiReading), cPtr);
  Paint_DrawString_EN(10, 0, cPtr, &Font16, BLACK, BLUE);
  updateText("MQ135:" + String(MQReading), cPtr);
  Paint_DrawString_EN(10, 30, cPtr, &Font16, BLACK, BLUE);
  updateText("PMS:", cPtr);
  Paint_DrawString_EN(10, 45, cPtr, &Font16, BLACK, BLUE);
  updateText(String(data.particles_03um), cPtr);
  Paint_DrawString_EN(10, 60, cPtr, &Font12, BLACK, BLUE);
  updateText(String(data.particles_05um), cPtr);
  Paint_DrawString_EN(45, 60, cPtr, &Font12, BLACK, BLUE);
  updateText(String(data.particles_10um), cPtr);
  Paint_DrawString_EN(80, 60, cPtr, &Font12, BLACK, BLUE);
  updateText("BME:" + String(humid), cPtr);
  Paint_DrawString_EN(10, 75, cPtr, &Font16, BLACK, BLUE);
  updateText("Grove:" + String(AQ), cPtr);
  Paint_DrawString_EN(10, 90, cPtr, &Font16, BLACK, BLUE);
  delay(2500); // Add delay for readability  
  OLED_1in5_rgb_Clear(); 
  
}