/*
REQUIRED SENSOR CONNECTIONS!!

Grove AQ Sensor = A0
Taidacent TTL = A1
MQ-135 = A2
BME280 = A4/A5
PMS5003 = 2/3

OLED CONNECTIONS:
	CS	  10
	DC	  7
	RST	  8	
	DIN	  11	
	CLK	  13

*/

// OLED Libraries
#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"


char message[20] = ""; // Message is super long so there are extra character spaces 
char* cPtr = message; // Pointer to the message array

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

int getTextColor (float yourIndex) {
  if (yourIndex > 1.9) {
        return RED;
    } else if (yourIndex < 1.2) {
        return GREEN;
    } else if (yourIndex < 1.4) {
        return YELLOW;
    } else if (yourIndex < 1.9) {
        return BROWN;
    } else {
      return GRAY;
    }

}

// BME280 Pressure Sensor Libraries
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;



//Grove Air Quality Sensor Libraries 
#include "Air_Quality_Sensor.h"
AirQualitySensor sensor(A0);

// DEVMO PMS5003 Libraries
#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(2, 3); 

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;


// Taidacent TTL
const int taidacent_input = A1;

// MQ-135 
const int MQ_input = A2;

//Establish your baselines here:
const int taidacent_baseline = 210;
const int MQ_baseline = 280;
  //DEVMO PMS5003 baselines:
    int particles_03um_baseline = 430;
    int particles_05um_baseline = 350;
const int humidity_baseline = 50;
// Where to store the particlces info for PMS5003
    float particles_03um = 0;
    float particles_05um = 0;


// no grove air quality baseline as it already catagorizes input into levels


// To be calculated into risk index: 
  int taidacent_index = 1;
  int MQ_index = 1;
  int PMS_baseline = 1; // or grove dust sensor

// Risk Index 
int risk_index = 0;

void setup() {
  Serial.begin(9600);

  // PMS5003 Baud rate
    pmsSerial.begin(9600);

  Serial.println(F("Waiting sensor to init..."));
  //Checking if Grove Air Quality Sensor is ready
    if (sensor.init()) {
        Serial.println("Grove Air Quality sensor ready.");
    } else {
        Serial.println("Grove Air Quality sensor ERROR!");
    }

  //Initialize BME280 Sensor
     if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
  // OLED initialization 
    System_Init();
  if(USE_IIC) {
    Serial.print("Only USE_SPI_4W, Please revise DEV_config.h !!!");
    return 0;
  }
  
  Serial.print(F("OLED_Init()...\r\n"));
  OLED_1in5_rgb_Init();
  Driver_Delay_ms(500); 
  OLED_1in5_rgb_Clear();  
  
  //1.Create a new image size
  UBYTE *BlackImage;
  Serial.print("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
  Paint_SetScale(65);

}

void loop() {

  //Taidacent code:
    float taidacent_reading = analogRead(taidacent_input);

  //MQ-135 code:
    float MQ_reading = analogRead(MQ_input);

  //Grove Air quality sensor code:
    int quality = sensor.slope();

    float grove_AQ_risk = 1;
    if (quality == AirQualitySensor::FORCE_SIGNAL) {
        grove_AQ_risk = 1.9;
    } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
        grove_AQ_risk = 1.7;
    } else if (quality == AirQualitySensor::LOW_POLLUTION) {
        grove_AQ_risk = 1.4;
    } else if (quality == AirQualitySensor::FRESH_AIR) {
        grove_AQ_risk = 1.2;
    }
  
  // DEVMO PMS5003
      if (readPMSdata(&pmsSerial)) {
        particles_03um = data.particles_03um;
        particles_05um = data.particles_05um;
        Serial.println("inside if:" + String(particles_03um));
    }

  //BME280 Pressure/temp/humidity
    float humidity = bme.readHumidity();

  //Risk index calculations
    float pms5003_risk = ((particles_03um/particles_03um_baseline)
                      +(particles_05um/particles_05um_baseline)
                    )/2;

    float taidacent_risk = (taidacent_reading/taidacent_baseline);
    float MQ_risk = (MQ_reading/MQ_baseline);

    float humidity_risk = (humidity/humidity_baseline);

    float total_risk = (pms5003_risk+grove_AQ_risk+taidacent_risk+MQ_risk+humidity_risk)/5;

    // // determine which category 
    // Serial.println("---------------------------------------");
    // Serial.print("Risk Type: ");
    

    if (total_risk < 1.2) {
      Serial.println("Clean Air");
    } else if (total_risk < 1.4) {
      Serial.println("Moderate");
    } else if (total_risk <1.9) {
      Serial.println("Unhealthy");
    } else {
      Serial.println("Dangerous!!");
    };

    // Serial.println("----------------");
    // Serial.println("Risk index PMS: " + String(pms5003_risk));
    // Serial.println("Risk index Grove AQ: " + String(grove_AQ_risk));
    // Serial.println("Risk index Taidacent: " + String(taidacent_risk));
    // Serial.println("Risk index MQ-135: " + String(MQ_risk));
    // Serial.println("----------------");
    // Serial.println("Risk index total: " + String(total_risk));
    // Serial.println("Humidity: " + String(humidity) + "%");
    // Serial.println("---------------------------------------");

    Serial.println("Debug nation over here:" + String(humidity));

    //OLED Display code: 
        updateText("PMS5003:" + String(pms5003_risk), cPtr);
        Paint_DrawString_EN(10, 0, cPtr, &Font8, BLACK, getTextColor(pms5003_risk));
        updateText("Grove AQ:" + String(grove_AQ_risk), cPtr);
        Paint_DrawString_EN(10, 15, cPtr, &Font8, BLACK, getTextColor(grove_AQ_risk));
        updateText("Taidacent:" + String(taidacent_risk), cPtr);
        Paint_DrawString_EN(10, 30, cPtr, &Font8, BLACK, getTextColor(taidacent_risk));
        updateText("MQ-135:" + String(MQ_risk), cPtr);
        Paint_DrawString_EN(10, 45, cPtr, &Font8, BLACK, getTextColor(MQ_risk));
        updateText("Humidity: " + String(humidity) + "%", cPtr);
        Paint_DrawString_EN(10, 60, cPtr, &Font8, BLACK, getTextColor(humidity_risk));
        updateText("----------------", cPtr);
        Paint_DrawString_EN(10, 75, cPtr, &Font8, BLACK, GRAY);
        updateText("Total risk index:" + String(total_risk), cPtr);
        Paint_DrawString_EN(10, 90, cPtr, &Font8, BLACK, getTextColor(total_risk));
        delay(2500); // Add delay for readability  
        OLED_1in5_rgb_Clear();   
     





}

  // Additional code for the DEVMO PMS5003
    boolean readPMSdata(Stream *s) {
      if (! s->available()) {
        return false;
      }
      
      // Read a byte at a time until we get to the special '0x42' start-byte
      if (s->peek() != 0x42) {
        s->read();
        return false;
      }
    
      // Now read all 32 bytes
      if (s->available() < 32) {
        return false;
      }
        
      uint8_t buffer[32];    
      uint16_t sum = 0;
      s->readBytes(buffer, 32);
    
      // get checksum ready
      for (uint8_t i=0; i<30; i++) {
        sum += buffer[i];
      }
          
      // The data comes in endian'd, this solves it so it works on all platforms
      uint16_t buffer_u16[15];
      for (uint8_t i=0; i<15; i++) {
        buffer_u16[i] = buffer[2 + i*2 + 1];
        buffer_u16[i] += (buffer[2 + i*2] << 8);
      }
    
      // put it into a nice struct :)
      memcpy((void *)&data, (void *)buffer_u16, 30);
    
      if (sum != data.checksum) {
        Serial.println("Checksum failure PMS5003");
        return false;
      }
      // success!
      return true;
    }
