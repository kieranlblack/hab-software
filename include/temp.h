#ifndef TEMP_H
#define TEMP_H

extern int temp_int_mv;
extern double temp_int;
extern int temp_ext_mv;
extern double temp_ext;

bool setup_temp();
bool read_temp();

#endif
