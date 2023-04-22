








#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>



#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;


 float highestReading = 0;

 int waitTime = 3000;



U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2 (U8G2_R0);



void setup() {

u8g2.begin();

  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = -8768.42; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = round(LoadCell.getData() * 10.0) / 10.0;
      
      newDataReady = 0;
      t = millis();

 
  if (i > highestReading){
         highestReading = i;
       }
 
 
 
 if (i < highestReading){

static unsigned long startTime = millis();
unsigned long currentTime = millis();
// timer if statement
if (currentTime - startTime > waitTime){

 highestReading = i;
// reset the timer
  startTime = millis();

  
}

       
       }

       


 


      

       String weight = String(highestReading, 0) + " lbs"; // Read the value from the Serial Monitor and store it in the variable

     
  

   u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_logisoso32_tr);	// choose a suitable font
  u8g2.drawStr(0,32, weight.c_str());	// write something to the internal memory
  u8g2.sendBuffer();	

Serial.println(highestReading);
    	
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }



  

 
}
