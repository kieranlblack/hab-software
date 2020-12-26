#include <Arduino.h>
#include <Adafruit_MPRLS.h>
#include <AltSoftSerial.h>
#include <DHT.h>
#include <SDFat.h>
#include <SPI.h>
#include <TinyGPS++.h>

void append_to_sd(char *str);
float get_temp(uint8_t enable_pin, uint8_t temp_pin);
inline uint32_t get_voltage();
inline bool is_buzz_time();
inline void log_to_sd();
inline void read_gps();
void smart_delay(uint32_t ms);

#define DEBUG false
const bool BUZZER = false;

const char LOG_FILENAME[] = "OUT.LOG";
const char DELIMITER = ',';

const uint8_t PIN_SD_CS = 6;
const uint8_t PIN_TEMP_INT_EN = 4;
const uint8_t PIN_TEMP_EXT_EN = 5;
const uint8_t PIN_TEMP_INT = A1;
const uint8_t PIN_TEMP_EXT = A2;
const uint8_t PIN_BUZZER = 3;
const uint8_t PIN_VOLTMETER = A3;
const uint8_t PIN_DHT = 2;

const uint32_t VOLTMETER_R1 = 10000;
const uint32_t VOLTMETER_R2 = 3300;

SdFat sd;
SdFile active_file;

Adafruit_MPRLS mprls = Adafruit_MPRLS(-1, -1);
DHT dht;

float pressure_mprls = 0;
float temp_ext = 0;
float temp_int = 0;
float humidity = 0;
float temp_dht = 0;
uint16_t vin = 0;

AltSoftSerial ss;
TinyGPSPlus gps;

double latitude = 0;
double longitude = 0;
double course = 0;
float altitude = 0;
float speed = 0;
uint32_t gps_time = 0;
uint8_t sat_count = 0;
uint32_t age = 0;

uint32_t time = 0;

inline bool is_buzz_time() {
    return BUZZER && (pressure_mprls > 900 || altitude < 3000);
}

// return temperature in celsius
float get_temp(uint8_t enable_pin, uint8_t temp_pin) {
    digitalWrite(enable_pin, HIGH);
    analogReference(INTERNAL);
    analogRead(temp_pin);
    delay(10);
    vin = analogRead(temp_pin);
    digitalWrite(enable_pin, LOW);

    return ((4L + (1100L * vin / 1024L) - 424) / 25);
}

// return  voltage in millivolts
inline uint32_t get_voltage() {
    analogReference(DEFAULT);
    analogRead(PIN_VOLTMETER);
    delay(10);

    return (uint32_t)(5000L * analogRead(PIN_VOLTMETER) / 1024L) * (VOLTMETER_R1 + VOLTMETER_R2) / VOLTMETER_R2;
}

// read all values from gps
inline void read_gps() {
    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        age = gps.location.age();
    }
    if (gps.altitude.isValid()) altitude = gps.altitude.meters();
    if (gps.speed.isValid()) speed = gps.speed.mps();
    if (gps.satellites.isValid()) sat_count = gps.satellites.value();
    if (gps.course.isValid()) course = gps.course.deg();
    if (gps.time.isValid()) gps_time = gps.time.value() / 100; //TODO: check this removed the centiseconds
}

void smart_delay(uint32_t ms) {
    uint32_t start = millis();

    do {
        while (ss.available() > 0) gps.encode(ss.read());
    } while (millis() - start < ms);
}

void setup() {
    ss.begin(9600);

    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_TEMP_EXT_EN, OUTPUT);
    pinMode(PIN_TEMP_INT_EN, OUTPUT);
#if DEBUG
    Serial.begin(9600);

    if (mprls.begin()) {
        Serial.println(F("MPRLS sensor found."));
    } else {
        Serial.println(F("Failed to find the MPRLS sensor."));
    }

    dht.setup(PIN_DHT);

    if (sd.begin(PIN_SD_CS, SPI_HALF_SPEED)) {
        Serial.println(F("SD card is ready to use."));
    } else {
        Serial.println(F("SD card initialization failed."));

        sd.initErrorHalt();
    }

    Serial.println(F("T,GT,PRES,HUM,TDHT,TIN,TEXT,LAT,LNG,ALT,SPD,CRS,CNT,V,AGE"));
#else
    mprls.begin();
    dht.setup(PIN_DHT);

    if (!sd.begin(PIN_SD_CS, SPI_HALF_SPEED)) sd.initErrorHalt();
#endif
    if (active_file.open(LOG_FILENAME, O_RDWR | O_CREAT | O_AT_END)) {
        active_file.println(F("t,gps_t,pressure,humidity,temp_dht,temp_int,temp_ext,lat,long,altitude,speed,course,sat_count,mV,age"));
        active_file.close();
    } else {
#if DEBUG
        Serial.print(F("Error opening "));
        Serial.println(LOG_FILENAME);
#endif
    }

    smart_delay(500);
}

void loop() {
    time = millis();

    noTone(PIN_BUZZER);
    if (is_buzz_time()) tone(PIN_BUZZER, 400);

    pressure_mprls = mprls.readPressure();
    humidity = dht.getHumidity();
    temp_dht = dht.getTemperature();
    temp_ext = get_temp(PIN_TEMP_EXT_EN, PIN_TEMP_EXT);
    temp_int = get_temp(PIN_TEMP_INT_EN, PIN_TEMP_INT);
    vin = get_voltage();
    read_gps();

    if (is_buzz_time()) tone(PIN_BUZZER, 1500);

    log_to_sd();
#if DEBUG
    Serial.print(time);
    Serial.print(DELIMITER);
    Serial.print(gps_time);
    Serial.print(DELIMITER);
    Serial.print(pressure_mprls);
    Serial.print(DELIMITER);
    Serial.print(humidity);
    Serial.print(DELIMITER);
    Serial.print(temp_dht);
    Serial.print(DELIMITER);
    Serial.print(temp_int);
    Serial.print(DELIMITER);
    Serial.print(temp_ext);
    Serial.print(DELIMITER);
    Serial.print(latitude, 7);
    Serial.print(DELIMITER);
    Serial.print(longitude, 7);
    Serial.print(DELIMITER);
    Serial.print(altitude);
    Serial.print(DELIMITER);
    Serial.print(speed);
    Serial.print(DELIMITER);
    Serial.print(course);
    Serial.print(DELIMITER);
    Serial.print(sat_count);
    Serial.print(DELIMITER);
    Serial.print(vin);
    Serial.print(DELIMITER);
    Serial.print(age);
    Serial.println();
#endif
    smart_delay(2000);
}

inline void log_to_sd() {
    active_file.open(LOG_FILENAME, O_RDWR | O_CREAT | O_AT_END);

    active_file.print(time);
    active_file.print(DELIMITER);
    active_file.print(gps_time);
    active_file.print(DELIMITER);
    active_file.print(pressure_mprls);
    active_file.print(DELIMITER);
    active_file.print(humidity);
    active_file.print(DELIMITER);
    active_file.print(temp_dht);
    active_file.print(DELIMITER);
    active_file.print(temp_int);
    active_file.print(DELIMITER);
    active_file.print(temp_ext);
    active_file.print(DELIMITER);
    active_file.print(latitude, 7);
    active_file.print(DELIMITER);
    active_file.print(longitude, 7);
    active_file.print(DELIMITER);
    active_file.print(altitude);
    active_file.print(DELIMITER);
    active_file.print(speed);
    active_file.print(DELIMITER);
    active_file.print(course);
    active_file.print(DELIMITER);
    active_file.print(sat_count);
    active_file.print(DELIMITER);
    active_file.print(vin);
    active_file.print(DELIMITER);
    active_file.print(age);
    active_file.println();

    active_file.close();
}
