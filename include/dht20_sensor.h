#ifndef DHT20_SENSOR_H
#define DHT20_SENSOR_H

extern double humidity;
extern double temp_dht;

bool setup_dht20();
bool read_dht20();

#endif
