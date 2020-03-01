#include <Arduino.h>
// #include <Adafruit_BMP280.h>
#include <Adafruit_MPRLS.h>
#include <AltSoftSerial.h>
#include <DHT.h>
#include <SDFat.h>
#include <SPI.h>
#include <TinyGPS++.h>

#define DEBUG false
static const bool BUZZER = true;

// Names for the log file
static const char LOGFILE[8] = "OUT.LOG";

static const char* DELIMITER = ",";

static const uint8_t PINCS = 6; // Pin for the SD Card
static const uint8_t PINRTEMP_IN = 4; // Reference pins for the temperature sensors
static const uint8_t PINRTEMP_EXT = 5;
static const uint8_t PINTEMP_IN = A1; // Pins for the LM60 temperature sensors
static const uint8_t PINTEMP_EXT = A2;
static const uint8_t PINBUZZ = 3; // Pin for the buzzer
static const uint8_t PINVMETER = A3; // Pin for the voltmeter
static const uint8_t PINDHT = 2;

static const uint32_t VMETER_R1 = 10000; // Resistance of the first resistor
static const uint32_t VMETER_R2 = 3300;

static SdFat sd;
static SdFile activeFile;

// static Adafruit_BMP280 bmp = Adafruit_BMP280();
static Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);
static DHT dht;

static AltSoftSerial ss; // Need to use pins 8 and 9 for RX and TX respectively

static TinyGPSPlus gps;

// All the variables for sensor data
static float pressure_MPRLS;
// static float pressure_BMP;
// static float temp_BMP;
static float tempext;
static float tempin;
static float humidity;
static float dht_temp;
static uint16_t vin;

// All the variables for the GPS data
static double latitude;
static double longitude;
static double course;
static float alt;
static float speed;
static uint32_t gpsTime;
static uint8_t satCount;
static uint32_t age;

static uint32_t time; // Time since the program started

static bool buzzTime() { return BUZZER && pressure_MPRLS > 900; };

static double getTemp(uint8_t refPin, uint8_t tempPin) {
    // Will calculate the temperature based off the voltage and return it in C
    digitalWrite(refPin, HIGH); // Turn on the LM60
    analogReference(INTERNAL); // Set the reference
    analogRead(tempPin); // Trash the bad reading
    delay(10); // Wait for the switch
    vin = analogRead(tempPin); // Get the real reading
    digitalWrite(refPin, LOW); // Turn off the LM60

    return (4L + (1100L * vin / 1024L) - 424) / 25;
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

    pinMode(PINRTEMP_EXT, OUTPUT); // Set up temperature enable pins
    pinMode(PINRTEMP_IN, OUTPUT);
#if DEBUG
    // // Check if the BMP sensor is connected
    // if (bmp.begin()) {
    //     Serial.println(F("BMP sensor found."));
    // } else {
    //     Serial.println(F("Failed to find the BMP sensor."));
    // }

    // Check if the MPRLS sensor is connected
    if (mpr.begin()) {
        Serial.println(F("MPRLS sensor found."));
    } else {
        Serial.println(F("Failed to find the MPRLS sensor."));
    }

    if (sd.begin(PINCS, SPI_HALF_SPEED)) {
        Serial.println(F("SD card is ready to use."));
    } else {
        Serial.println(F("SD card initialization failed."));

        return;
    }
#else
    // Initialise everything
    // bmp.begin();
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

void loop() {
    time = millis();
    noTone(PINBUZZ);

    if (buzzTime()) tone(PINBUZZ, 400, 1000);

    // Get the sensor data
    // pressure_BMP = bmp.readPressure() / 100; // This function is absolutely massive for some reason
    // temp_BMP = bmp.readTemperature();
    pressure_MPRLS = mpr.readPressure();
    humidity = dht.getHumidity();
    dht_temp = dht.getTemperature();
    tempext = getTemp(PINRTEMP_EXT, PINTEMP_EXT);
    tempin = getTemp(PINRTEMP_IN, PINTEMP_IN);

    analogReference(DEFAULT); // Make sure we will be reading with a good reference voltage
    analogRead(PINVMETER); // Trash the first reading
    delay(10); // Give the arduino time to make the switch
    vin = 5000L * analogRead(PINVMETER) / 1024L * (VMETER_R1 + VMETER_R2) / VMETER_R2;

    // Get the GPS data
    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
    if (gps.altitude.isValid()) alt = gps.altitude.meters();
    if (gps.speed.isValid()) speed = gps.speed.mps();
    if (gps.satellites.isValid()) satCount = gps.satellites.value();
    if (gps.course.isValid()) course = gps.course.deg();
    if (gps.time.isValid()) gpsTime = gps.time.value();
    age = gps.time.age();

    if (buzzTime()) tone(PINBUZZ, 1500, 1000);

    // We write the data to the SD card
    activeFile.open(LOGFILE, O_RDWR | O_CREAT | O_AT_END);

    // Log all the data to the file, yeah it is ugly but it saves memory, if memory is REALLY tight consider casting all of these to doubles first to avoid overloading the function
    // activeFile.println(outBuff);
    activeFile.print(time);
    activeFile.print(DELIMITER);
    activeFile.print(gpsTime);
    activeFile.print(DELIMITER);
    // activeFile.print(pressure_BMP);
    // activeFile.print(DELIMITER);
    activeFile.print(pressure_MPRLS);
    activeFile.print(DELIMITER);
    activeFile.print(humidity);
    activeFile.print(DELIMITER);
    activeFile.print(dht_temp);
    activeFile.print(DELIMITER);
    // activeFile.print(temp_BMP);
    // activeFile.print(DELIMITER);
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
    Serial.print(pressure_MPRLS);
    Serial.print(DELIMITER);
    Serial.print(humidity);
    Serial.print(DELIMITER);
    Serial.print(dht_temp);
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

    smartDelay(1000); // Something to note is that although the DHT22 is a lot more accurate we can only read from it once every 2 seconds
}
