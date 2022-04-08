#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG        true // comment this line out to disable
#define DEBUG_STREAM Serial

#define PIN_SD_CS    6
#define SD_LOG_FILE  "apple.log"
#define DELIMITER    F(",")

#define PIN_TEMP_INT_EN 4
#define PIN_TEMP_INT    A1
#define PIN_TEMP_EXT_EN 5
#define PIN_TEMP_EXT    A2

#define PIN_BUTTON 2
#define PIN_BUZZER_MOSFET 3
#define BUZZ_ALT          3000

#endif
