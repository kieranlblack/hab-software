# Which pin goes where?
## General Pins
| Arduino  | Sensor           |
|:--------:|:----------------:|
| A1       | Internal Temp    |
| A2       | External Temp    |
| A3       | Voltage Pin      |
| A4       | SDA              |
| A5       | SCL              |
| 2        | DHT              |
| 3        | Buzzer           |
| 4        | Internal LM60 EN |
| 5        | External LM60 EN |
| 6        | SD Card CS       |
| 8        | GPS TX           |
| 11       | SD Card MOSI     |
| 12       | SD Card MISO     |
| 13       | SD Card SCK      |

## I2C Stuff
Just link all the SCL and SDA pins and make sure to put 10k pullups on the SCL and SDA lines. 

# Symbol Meanings
| Short | Long                  | Units   | Notes                   |
|:-----:|:--------------------:|:--------:|:----------------------- |
| T     | Time Elapsed         | ms       | millis()                |
| GT    | Current Time         | HHMMSS   | UTC                     |
| PRES  | Pressure             | hPa      |                         |
| HUM   | Humidity             | %        | Relative Humidity       |
| TDHT  | DHT Temperature      | C        |                         |
| TIN   | Internal Temperature | C        |                         |
| TEXT  | External Temperature | C        |                         |
| LAT   | Latitude             | degrees  | Can be converted to DMS |
| LNG   | Longitude            | degrees  |                         |
| ALT   | Altitude             | m        |                         |
| SPD   | Speed                | m/s      |                         |
| CRS   | Course               | degrees  |                         |
| CNT   | Satellite Count      | number   |                         |
| V     | Voltage              | mV       |                         |
| AGE   | Age of GPS Data      | ms       |                         |

# Extra Info
## LM60
If the LM60 is connected backwards it will get really hot. \
[LM60 datasheet](http://pdf.datasheetcatalog.com/datasheet/nationalsemiconductor/LM60.pdf)
## GPS Stuff
Time to first fix from a cold start is about 26 seconds. \
[Breakout board datasheet](https://store.uputronics.com/files/HAB-BO-M8Q3A-ASSY.pdf) \
[MAX-M8Q datasheet](https://www.u-blox.com/sites/default/files/MAX-M8-FW3_DataSheet_%28UBX-15031506%29.pdf)
## MPRLS
[MPRLS breakout information](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-mprls-ported-pressure-sensor-breakout.pdf) \
[MPR datasheet](https://sensing.honeywell.com/honeywell-sensing-micropressure-board-mount-pressure-mpr-series-datasheet-32332628.pdf)
## DHT22
Toss a 10k pull up resistor linking the data line to power pin. Power it with 3.3v or it will heat up and throw off temperature data. \
[DHT22 datasheet](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)
## SD Card Breakout
[SD card guide](https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/arduino-wiring)
## BMP280
Library for this bad boy is huge. \
[BMP280 datasheet](https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf)
## HX1
[Hookup guide](https://learn.sparkfun.com/tutorials/hx1-aprs-transmitter-hookup-guide/all) \
[HX1 datasheet](https://cdn.sparkfun.com/assets/d/1/1/4/b/HX1_APRS.pdf)