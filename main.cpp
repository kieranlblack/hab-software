#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPRLS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>

static const String TEMPFILE = "TEMP.LOG"; // Name for the temperature sensor log file
static const int PINCS = 8; // Pin 10

// Serial pins for the gps
static const int PINRX = 4;
static const int PINTX = 3;

// Pins for the LM60 temperature sensor
static const int PINTEMP_IN = A1;
static const int PINTEMP_EXT = A2;

static const float _hPa_PSI = 68.947572932;

File myFile;
Adafruit_BMP280 bmp = Adafruit_BMP280();
Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);

SoftwareSerial ss(PINRX, PINTX);

TinyGPSPlus gps;

float pressure_MPRLS;
float pressure_BPM;
float temp;
float latitude;
float longitude;
float alt;

void setup() {
    Serial.begin(9600); // Start serial for output
    ss.begin(9600); // Start the gps serial

    pinMode(PINCS, OUTPUT); // Set pinCS as an output pin (SD card data output)

    // Check if the BMP sensor is connected
    if (bmp.begin()) {
        Serial.println("BMP sensor found.");
    } else {
        Serial.println("Failed to find the BMP sensor.");
    }

    // Check if the MPRLS sensor is connected
    if (mpr.begin()) {
        Serial.println("MPRLS sensor found.");
    } else {
        Serial.println("Failed to find the MPRLS sensor.");
    }

    // Check if the GPS is connected
    if (ss.available() > 0) {
        Serial.println("GPS found.");
    } else {
        Serial.println("GPS not found.");
    }

    // SD card initialization
    if (SD.begin()) {
        Serial.println("SD card is ready to use.");
    } else {
        Serial.println("SD card initialization failed.");

        return;
    }

    myFile = SD.open(TEMPFILE.c_str(), FILE_WRITE); // Open the file or create it if it does not exist

    // Check if the file opened okay
    if (myFile) {
        // Yay it opened! now we write data to it
        Serial.println("Writing to file...");
        myFile.println("Data Logging....\nPressure and Temperature"); // Write to the file
        myFile.close(); // Close the file
        Serial.println("File closed.");
    } else {
        Serial.println("Error opening " + TEMPFILE);
    }
}

void loop() {
    pressure_BPM = bmp.readPressure() / 100;
    temp = bmp.readTemperature();
    pressure_MPRLS = mpr.readPressure();

    // Try and read from the GPS module
    if (ss.available() > 0) {
        gps.encode(ss.read());
        
        if (gps.location.isUpdated()) {
            latitude = gps.location.lat();
            longitude = gps.location.lng();
            alt = gps.altitude.meters();
        }
    }

    String gpsOutput = "Lat: " + String(latitude) + " Long: " + String(longitude) + " | Altitude: " + String(alt);
    String sensorOutput = "Pressure_BMP: " +  String(pressure_BPM) + " / Pressure_MPRLS: " + String(pressure_MPRLS) + " | Temp: " + String(temp);
    Serial.println(sensorOutput);
    Serial.println(gpsOutput);
    Serial.println();

    myFile = SD.open(TEMPFILE.c_str(), FILE_WRITE); // We write the data to the SD card
    myFile.println(sensorOutput);
    myFile.println(gpsOutput);
    myFile.println();

    myFile.close();
    delay(1000);
}
