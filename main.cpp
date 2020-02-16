#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPRLS.h>
#include <AltSoftSerial.h>
#include <SDFat.h> // #include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>

#define DEBUG false
static const bool BUZZER = true;

// Names for the log files
static const char* SENSFILE = "SENS.LOG";
static const char* GPSFILE = "GPS.LOG";

static const uint8_t PINCS = 8; // Pin for the SD Card
static const uint8_t PINTEMP_IN = A1; // Pins for the LM60 temperature sensors
static const uint8_t PINTEMP_EXT = A2;
static const uint8_t PINBUZZ = 3; // Pin for the buzzer

static SdFat sd;
static SdFile activeFile;
// static File activeFile;

static Adafruit_BMP280 bmp = Adafruit_BMP280();
static Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);

static AltSoftSerial ss; // Need to use pins 8 and 9 for RX and TX respectively

static TinyGPSPlus gps;

// All the variables for sensor data, why are they doubles you might ask... the compiler will yell if they are not
static double pressure_MPRLS;
static double pressure_BMP;
static double temp_BMP;
static double tempext;
static double tempin;

// All the variables for the GPS data
static double latitude;
static double longitude;
static double alt;
static double speed;
static uint32_t gpsTime;
static uint8_t satCount;

// The buffers we want to write to the files
static char* sensorOutBuff;
static char* gpsOutBuff;

static uint32_t time; // Holds the time since the program started

static auto buzzTime = [&]() { return BUZZER && (pressure_BMP > 90000 || pressure_MPRLS > 900); };

static double fixTemp(int sensorValue) {
    // Will calculate the temperature based off the voltage and return it in C 
    return ((((5.0 * sensorValue) / 1024) * 1000) - 424) / 6.25;
}

static void smartDelay(uint32_t ms) {
    // This will continue to read the gps data and then continue to even when the arduino is delaying
    uint32_t start = millis();

    do {
        while (ss.available() > 0) gps.encode(ss.read());
    } while (millis() - start < ms);
}

void setup() {
    Serial.begin(9600); // Start serial for output
    ss.begin(9600); // Start the gps serial

    // pinMode(PINCS, OUTPUT); // Set pinCS as an output pin (SD card data output)
    pinMode(PINBUZZ, OUTPUT); // Set up the buzzer pin as an output

    if (!sd.begin(PINCS, SPI_HALF_SPEED)) sd.initErrorHalt();
#if DEBUG
    // Check if the BMP sensor is connected
    if (bmp.begin()) {
        Serial.println(F("BMP sensor found."));
    } else {
        Serial.println(F("Failed to find the BMP sensor."));
    }

    // Check if the MPRLS sensor is connected
    if (mpr.begin()) {
        Serial.println(F("MPRLS sensor found."));
    } else {
        Serial.println(F("Failed to find the MPRLS sensor."));
    }

    // Check if the GPS is connected
    if (ss.available() > 0) {
        Serial.println(F("GPS found."));
    } else {
        Serial.println(F("GPS not found."));
    }

    // SD card initialization
    /* if (SD.begin()) {
        Serial.println(F("SD card is ready to use."));
    } else {
        Serial.println(F("SD card initialization failed."));

        return;
    } */

    if (sd.begin(PINCS, SPI_HALF_SPEED)) {
        Serial.println(F("SD card is ready to use."));
    } else {
        Serial.println(F("SD card initialization failed."));

        return;
    }
#else
    // Initialise everything
    bmp.begin();
    mpr.begin();
    // SD.begin();
#endif
    // activeFile = SD.open(SENSFILE, FILE_WRITE); // Open the file or create it if it does not exist
    activeFile.open(SENSFILE, O_RDWR | O_CREAT | O_AT_END);

    // Check if the sensor file opened okay
    if (true) { // activeFile
        // Yay it opened! now we write data to it
        activeFile.println(F("T,PBMP,PMPRLS,TBMP,TIN,TEXT")); // Write to the file
        activeFile.close(); // Close the file
    } else {
#if DEBUG
        Serial.println("Error opening " + String(SENSFILE));
#endif
    }

    // activeFile = SD.open(GPSFILE, FILE_WRITE);
    activeFile.open(GPSFILE, O_RDWR | O_CREAT | O_AT_END);

    if (true) { //activeFile
        activeFile.println(F("T,GT,LAT,LNG,ALT,SPD,CNT"));
        activeFile.close();
    } else {
#if DEBUG
        Serial.println("Error opening " + String(POSFILE));
#endif
    }
}

static void writeFile(const char* file, char* buff) {
    // activeFile = SD.open(file, FILE_WRITE);
    activeFile.open(file, O_RDWR | O_CREAT | O_AT_END);
    activeFile.println(buff);
    activeFile.close();
}

void loop() {
    time = millis();
    noTone(PINBUZZ);

    if (buzzTime()) tone(PINBUZZ, 400, 1000);

    // Get the sensor data
    pressure_BMP = bmp.readPressure() / 100;
    temp_BMP = bmp.readTemperature();
    pressure_MPRLS = mpr.readPressure();
    tempext = fixTemp(analogRead(PINTEMP_EXT));
    tempin = fixTemp(analogRead(PINTEMP_IN));

    sprintf(sensorOutBuff, "%lu,%f,%f,%f,%f,%f", time, pressure_BMP, pressure_MPRLS, temp_BMP, tempin, tempext);

    // Get the GPS data
    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
    if (gps.altitude.isValid()) alt = gps.altitude.meters();
    if (gps.speed.isValid()) speed = gps.speed.mps();
    if (gps.satellites.isValid()) gps.satellites.value();
    if (gps.time.isValid()) gpsTime = gps.time.value();
    
    sprintf(gpsOutBuff, "%lu,%lu,%f,%f,%f,%f,%d", time, gpsTime, latitude, longitude, alt, speed, satCount);

    if (buzzTime()) tone(PINBUZZ, 1500, 1000);
#if DEBUG
    Serial.println(sensorOutBuff);
    Serial.println(gpsOutBuff);
    Serial.println();
#endif
    // We write the data to the SD card
    writeFile(SENSFILE, sensorOutBuff);
    writeFile(GPSFILE, gpsOutBuff);

    smartDelay(1000);
}
