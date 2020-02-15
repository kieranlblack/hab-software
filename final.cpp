#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPRLS.h>
#include <SD.h>
#include <SPI.h>
//#include <TinyGPS++.h>
#include <SparkFun_I2C_GPS_Arduino_Library.h> //Use Library Manager or download here: https://github.com/sparkfun/SparkFun_I2C_GPS_Arduino_Library

/***
 * PLEASE, PLEASE CHANGE <SparkFun_I2C_GPS_Arduino_Library.h> 
 * TO USE THE I2C ADDRESS FOR THE GPS
 * Need to edit a line in file SparkFun_I2C_GPS_Arduino_Library.h
  #define MT333x_ADDR 0x10 //7-bit unshifted default I2C Address
  0x10 is the address of the sparkfun board; our breakout board’s address is 0x42
  so just change that and save the file
 */

#define buzzer false  // TURN ON FOR FLIGHT
#define buzzerPin 3


const String TEMPFILE = "2020.LOG"; // Name for the temperature sensor log file
const int PINCS = 8; // Pin 8 for nano

File myFile;
Adafruit_BMP280 bmp = Adafruit_BMP280();

Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);

I2CGPS myI2CGPS; //Hook object to the library

// The TinyGPS++ object
//TinyGPSPlus gps;


float pressure_MPRLS;
float pressure_BMP;
float temp;
float latitude;
float longitude;
float alt;

String sensorOutput;
String gpsOutput;

void setup() {
    Serial.begin(9600); // Start serial for output

    myI2CGPS.begin();  // Start I2C for GPS

    pinMode(PINCS, OUTPUT); // Set pinCS as an output pin (SD card data output)

    pinMode(buzzerPin, OUTPUT);

    bmp.begin();

    mpr.begin();

    // SD card initialization
    if (SD.begin()) {
        Serial.println("SD");
    } else {
        Serial.println("SD FAIL");

        return;
    }

    myFile = SD.open(TEMPFILE.c_str(), FILE_WRITE); // Open the file or create it if it does not exist

    // Check if the file opened okay
    if (myFile) {
        // Yay it opened! now we write data to it
        Serial.println("W");
        myFile.println("start"); // Write to the file
        myFile.close(); // Close the file
        Serial.println("C");
    } else {
        Serial.println("ERROR " + TEMPFILE);
    }
}

void loop() {

    if (playBuzzer()) {
      noTone(buzzerPin);
      tone(buzzerPin, 400, 1000);
    }
    
    pressure_MPRLS = mpr.readPressure();
    delay(200);
    pressure_BMP = bmp.readPressure();
    delay(200);
    temp = bmp.readTemperature();

    myFile = SD.open(TEMPFILE.c_str(), FILE_WRITE); // We write the data to the SD card

    if (myFile) {

      if (playBuzzer()) {
        tone(buzzerPin, 1500, 1000);
      }

      String lineOutput = "";

      lineOutput = String(pressure_MPRLS) 
      + ", " + String(pressure_BMP) + ", " + String(temp);
    
      myFile.println(lineOutput);
      Serial.println(lineOutput);

      while (myI2CGPS.available()) //available() returns the number of new bytes available from the GPS module
      {
        char c = myI2CGPS.read();
        if ((String)c == "$") {
          Serial.println();
          myFile.println();
        }
        Serial.print(c);
        myFile.print(c);
      }
      myFile.println();
      
      Serial.println("^y");
  
      myFile.close();
    }
    delay(1000);
}

bool playBuzzer() {
  if (buzzer) {
    if (pressure_BMP > 90000 || pressure_MPRLS > 900) {
      return true;
    }
  }
  return false;
}
