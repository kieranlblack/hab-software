#include <Arduino.h>
#include <Adafruit_MPRLS.h>
#include <AltSoftSerial.h>
#include <DHT.h>
#include <SDFat.h>
#include <SPI.h>
#include <TinyGPS++.h>

#define DEBUG true
const bool BUZZER = false;

// Name for the log file
const char LOGFILE[] = "AOUT.LOG";
const char DELIMITER = ',';

// Pins
const uint8_t PINCS = 6; // Pin for the SD Card
const uint8_t PINRTEMP_IN = 4; // Reference pins for the temperature sensors
const uint8_t PINRTEMP_EXT = 5;
const uint8_t PINTEMP_IN = A1; // Pins for the LM60 temperature sensors
const uint8_t PINTEMP_EXT = A2;
const uint8_t PINBUZZ = 3; // Pin for the buzzer
const uint8_t PINVMETER = A3; // Pin for the voltmeter
const uint8_t PINDHT = 2;

const uint32_t VMETER_R1 = 10000; // Resistance of the first resistor
const uint32_t VMETER_R2 = 3300;

const float tempCalibration = 0.0;

SdFat sd;
SdFile activeFile;

Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);
DHT dht;

AltSoftSerial ss; // Need to use pins 8 and 9 for RX and TX respectively

TinyGPSPlus gps;

// All the variables for sensor data
float pressureMPRLS;
float tempext;
float tempin;
float humidity;
float tempDHT;
uint16_t vin;

// All the variables for the GPS data
double latitude;
double longitude;
double course;
float alt;
float speed;
uint32_t gpsTime;
uint8_t satCount;
uint32_t age;

uint32_t time; // Time since the program started

inline bool buzzTime(void) { return BUZZER && (pressureMPRLS > 900 || alt < 3000); }

float getTemp(uint8_t refPin, uint8_t tempPin) {
    // Will calculate the temperature based off the voltage and return it in C
    digitalWrite(refPin, HIGH); // Turn on the LM60
    analogReference(INTERNAL); // Set the reference
    analogRead(tempPin); // Trash the bad reading
    delay(10); // Wait for the switch
    vin = analogRead(tempPin); // Get the real reading
    digitalWrite(refPin, LOW); // Turn off the LM60

    return ((4L + (1100L * vin / 1024L) - 424) / 25) + tempCalibration;
}

void smartDelay(uint32_t ms) {
    // This will continue to read the gps data and then continue to even when the arduino is delaying
    uint32_t start = millis();

    do {
        //? might be possible for the arduino to get stuck reading from the gps if we up refresh rate on the u-blox although highly unlikely
        while (ss.available() > 0) gps.encode(ss.read());
    } while (millis() - start < ms);
}

void setup(void) {
    ss.begin(9600); // Start the gps serial

    pinMode(PINBUZZ, OUTPUT); // Set up the buzzer pin as an output
    pinMode(PINRTEMP_EXT, OUTPUT); // Set up temperature enable pins
    pinMode(PINRTEMP_IN, OUTPUT);
#if DEBUG
    Serial.begin(9600); // Start serial for output

    // Check if the MPRLS sensor is connected
    if (mpr.begin()) {
        Serial.println(F("MPRLS sensor found."));
    } else {
        Serial.println(F("Failed to find the MPRLS sensor."));
    }

    dht.setup(PINDHT);

    if (sd.begin(PINCS, SPI_HALF_SPEED)) {
        Serial.println(F("SD card is ready to use."));
    } else {
        Serial.println(F("SD card initialization failed."));

        sd.initErrorHalt();
    }

    Serial.println(F("T,GT,PRES,HUM,TDHT,TIN,TEXT,LAT,LNG,ALT,SPD,CRS,CNT,V,AGE"));
#else
    // Initialise everything
    mpr.begin();
    dht.setup(PINDHT);

    if (!sd.begin(PINCS, SPI_HALF_SPEED)) sd.initErrorHalt();
#endif
    // Try and open the file to write to it
    if (activeFile.open(LOGFILE, O_RDWR | O_CREAT | O_AT_END)) {
        // time since start, gps time, mprls pressure, humidity, dht temp, internal temp, external temp, latitude, longitude, altitude, speed, course, satellite count, voltage, age of data
        activeFile.println(F("T,GT,PRES,HUM,TDHT,TIN,TEXT,LAT,LNG,ALT,SPD,CRS,CNT,V,AGE")); // Write to the file
        activeFile.close(); // Close the file
    } else {
#if DEBUG
        Serial.print(F("Error opening "));
        Serial.println(LOGFILE);
#endif
    }

    smartDelay(500); // Give the GPS time to get some sort of lock before jumping into the loop if it had one before
}

void loop(void) {
    time = millis();
    noTone(PINBUZZ);

    if (buzzTime()) tone(PINBUZZ, 400, 1000);

    // Get the sensor data
    /**
    *! this line will hang the entire program if the mprls becomes unplugged
    *! the catch lies in twi_readFrom / twi_writeTo which is a blocking call, if the slave never responds the master never continues
    **/
    pressureMPRLS = mpr.readPressure();
    humidity = dht.getHumidity();
    tempDHT = dht.getTemperature();
    tempext = getTemp(PINRTEMP_EXT, PINTEMP_EXT);
    tempin = getTemp(PINRTEMP_IN, PINTEMP_IN);

    analogReference(DEFAULT); // Make sure we will be reading with a good reference voltage
    analogRead(PINVMETER); // Trash the first reading
    delay(10); // Give the arduino time to make the switch
    vin = (uint32_t)(5000L * analogRead(PINVMETER) / 1024L) * (VMETER_R1 + VMETER_R2) / VMETER_R2;

    // Get the GPS data
    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
    if (gps.altitude.isValid()) alt = gps.altitude.meters();
    if (gps.speed.isValid()) speed = gps.speed.mps();
    if (gps.satellites.isValid()) satCount = gps.satellites.value();
    if (gps.course.isValid()) course = gps.course.deg();
    if (gps.time.isValid()) gpsTime = gps.time.value() / 100; //TODO: check this removed the centiseconds
    age = gps.time.age();

    if (buzzTime()) tone(PINBUZZ, 1500, 1000);

    // We write the data to the SD card
    activeFile.open(LOGFILE, O_RDWR | O_CREAT | O_AT_END);

    // Log all the data to the file, yeah it is ugly but it saves memory, if memory is REALLY tight consider casting all of these to doubles first to avoid overloading the function
    activeFile.print(time);
    activeFile.print(DELIMITER);
    activeFile.print(gpsTime);
    activeFile.print(DELIMITER);
    activeFile.print(pressureMPRLS);
    activeFile.print(DELIMITER);
    activeFile.print(humidity);
    activeFile.print(DELIMITER);
    activeFile.print(tempDHT);
    activeFile.print(DELIMITER);
    activeFile.print(tempin);
    activeFile.print(DELIMITER);
    activeFile.print(tempext);
    activeFile.print(DELIMITER);
    activeFile.print(latitude, 7);
    activeFile.print(DELIMITER);
    activeFile.print(longitude, 7);
    activeFile.print(DELIMITER);
    activeFile.print(alt);
    activeFile.print(DELIMITER);
    activeFile.print(speed);
    activeFile.print(DELIMITER);
    activeFile.print(course);
    activeFile.print(DELIMITER);
    activeFile.print(satCount);
    activeFile.print(DELIMITER);
    activeFile.print(vin);
    activeFile.print(DELIMITER);
    activeFile.print(age);
    activeFile.println();

    activeFile.close();
#if DEBUG
    Serial.print(time);
    Serial.print(DELIMITER);
    Serial.print(gpsTime);
    Serial.print(DELIMITER);
    Serial.print(pressureMPRLS);
    Serial.print(DELIMITER);
    Serial.print(humidity);
    Serial.print(DELIMITER);
    Serial.print(tempDHT);
    Serial.print(DELIMITER);
    Serial.print(tempin);
    Serial.print(DELIMITER);
    Serial.print(tempext);
    Serial.print(DELIMITER);
    Serial.print(latitude, 7);
    Serial.print(DELIMITER);
    Serial.print(longitude, 7);
    Serial.print(DELIMITER);
    Serial.print(alt);
    Serial.print(DELIMITER);
    Serial.print(speed);
    Serial.print(DELIMITER);
    Serial.print(course);
    Serial.print(DELIMITER);
    Serial.print(satCount);
    Serial.print(DELIMITER);
    Serial.print(vin);
    Serial.print(DELIMITER);
    Serial.print(age);
    Serial.println();
#endif
    smartDelay(960); // Something to note is that although the DHT22 is a lot more accurate we can only read from it once every 2 seconds
}
