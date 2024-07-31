/*
REQUIRED SENSOR CONNECTIONS!!

Grove AQ Sensor = A0
Taidacent TTL = A1
MQ-135 = A2
BME280 = A4/A5
PMS5003 = 2/3

*/

// // OLED Libraries
// #include "OLED_Driver.h"
// #include "GUI_paint.h"
// #include "DEV_Config.h"
// #include "Debug.h"
// #include "ImageData.h"

// // BME280 Pressure Sensor Libraries
// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>

// #define SEALEVELPRESSURE_HPA (1013.25)

// Adafruit_BME280 bme;

// bool status;
// float humid;


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

int particles_03um_baseline = 500;
int particles_05um_baseline = 430;
int particles_10um_baseline = 50;
int particles_25um_baseline = 1;
int particles_50um_baseline = 1;
int particles_100um_baseline = 1;

    float particles_03um = 0;
    float particles_05um = 0;
    float particles_10um = 0;
    float particles_25um = 0;
    float particles_50um = 0;
    float particles_100um = 0;

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

  // Initializing inputs
  // pinMode(taidacent_input, INPUT);
  // pinMode(MQ_input, INPUT);

  Serial.println("Waiting sensor to init...");
    delay(1000);
  //Checking if Grove Air Quality Sensor is ready
    if (sensor.init()) {
        Serial.println("Grove Air Quality sensor ready.");
    } else {
        Serial.println("Grove Air Quality sensor ERROR!");
    }

//   //Initialize BME280 Sensor
//   status = bme.begin(); 
//   if (!status) {
//     Serial.println("Could not find a valid BME280 sensor, check wiring!");
//   }

}

void loop() {

  //Taidacent code:
 float taidacent_reading = analogRead(taidacent_input);

  //MQ-135 code:
  float MQ_reading = analogRead(MQ_input);

  //Grove Air quality sensor code:
  int quality = sensor.slope();

    // Serial.print("Sensor value: ");
    // Serial.println(sensor.getValue());
  float grove_AQ_risk = 1;
    if (quality == AirQualitySensor::FORCE_SIGNAL) {
        // Serial.println("Dangerous");
        grove_AQ_risk = 1.9;
    } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
        // Serial.println("Unhealthy");
        grove_AQ_risk = 1.7;
    } else if (quality == AirQualitySensor::LOW_POLLUTION) {
        // Serial.println("Moderate");
        grove_AQ_risk = 1.4;
    } else if (quality == AirQualitySensor::FRESH_AIR) {
        // Serial.println("Clean air");
        grove_AQ_risk = 1.2;
    }
  
  // DEVMO PMS5003
  if (readPMSdata(&pmsSerial)) {
    // Serial.println("---------------------------------------");
    // Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
    // Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
    // Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
    // Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
    // Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
    // Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
    // Serial.println("---------------------------------------");
    particles_03um = data.particles_03um;
    particles_05um = data.particles_05um;
    particles_10um = data.particles_10um;
    particles_25um = data.particles_25um;
    particles_50um = data.particles_50um;
    particles_100um = data.particles_100um;
  }

  // //BME280 Pressure/temp/humidity
  // humid = bme.readHumidity();
  // Serial.println(humid);

  //Risk index calculations
  float pms5003_risk = ((particles_03um/particles_03um_baseline)
                    +(particles_05um/particles_05um_baseline)
                   )/2;

  float taidacent_risk = (taidacent_reading/taidacent_baseline);
  float MQ_risk = (MQ_reading/MQ_baseline);

  float total_risk = (pms5003_risk+grove_AQ_risk+taidacent_risk+MQ_risk)/4;

  //determine which category 
Serial.println("---------------------------------------");
Serial.print("Risk Type: ");

  if (total_risk < 1.2) {
    Serial.println("Clean Air");
  } else if (total_risk < 1.4) {
    Serial.println("Moderate");
  } else if (total_risk <1.9) {
    Serial.println("Unhealthy");
  } else {
    Serial.println("Dangerous!!");
  };

  Serial.println("----------------");
  Serial.println("Risk index PMS: " + String(pms5003_risk));
  Serial.println("Risk index Grove AQ: " + String(grove_AQ_risk));
  Serial.println("Risk index Taidacent: " + String(taidacent_risk));
  Serial.println("Risk index MQ-135: " + String(MQ_risk));
  Serial.println("----------------");
  Serial.println("Risk index total: " + String(total_risk));
  Serial.println("---------------------------------------");


  //OLED Display code: 





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
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
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
