# eRTC

The eRTC component is a module for handling the real-time clock (RTC) on the ESP32. This component allows you to set and read the current time and date, as well as manage alarms and timers. 

## Features

- Set the current time and date.
- Read the current time and date.
- Set alarms.
- Manage timers.

## Dependencies

This module depends on the following components:
- [eI2C](https://github.com/acevedoesteban999/eI2C)

## Main Functions

- `void eRTC_init()`: Initializes the RTC module.
- `void eRTC_setTime(int year, int month, int day, int hour, int minute, int second)`: Sets the current time and date.
- `eRTC_Time eRTC_getTime()`: Returns the current time and date.
- `void eRTC_setAlarm(int hour, int minute, int second, void (*callback)())`: Sets an alarm that will trigger at the specified time and execute the callback function.
- `void eRTC_setTimer(int seconds, void (*callback)())`: Sets a timer that will trigger after the specified number of seconds and execute the callback function.

## Example Usage

```c
#include "eRTC.h"

void app_main() {
    // Initialize the RTC
    eRTC_init();

    // Set the current time and date
    eRTC_setTime(2023, 10, 15, 12, 0, 0);

    // Read the current time and date
    eRTC_Time time = eRTC_getTime();
    printf("Current date and time: %d-%d-%d %d:%d:%d\n", time.year, time.month, time.day, time.hour, time.minute, time.second);
}
```
