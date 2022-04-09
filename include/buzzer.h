#ifndef BUZZER_H
#define BUZZER_H

bool setup_buzz();
bool is_buzz_time(double altitude);
void toggle_buzzer_enable();
void tone_off_buzzer();
void flip_buzz_state();

#endif
