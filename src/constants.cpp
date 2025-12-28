// constants.cpp
#include "constants.h"

// Definitions for variables declared in constants.h
double minBrightness = 5;                     // min brightness for LEDs
double maxBrightness = 20;                    // maximum brightness for LEDs
double minAnalogvalue = 150;                  // min value from light sensor
double maxAnalogvalue = 4095;                 // max value from light sensor
double slope = (maxBrightness - minBrightness) / (maxAnalogvalue - minAnalogvalue);  // calculate slope (two point linear regression), m = (y2-y1) / (x2-x1)
double intercept = minBrightness - (slope * minAnalogvalue);                         // b = y1 - (m*x1)

unsigned long currentTime = 0;                // Current time (set at runtime)
unsigned long previousTime = 0;               // Previous time
const long timeoutTime = 2000;                // timeout time in milliseconds

unsigned long StartTimeCheck = 0;             // Previous time for start check
const long WaitTimeCheck = (1000 * 60 * 5);   // 5 minutes

int animRepeats = 5;                          // default repeat count for animations

// Network/server objects
WebServer server(80);
const char* ssid = "<<YOUR SSID>>";
const char* password = "<<YOUR PASSWORD>>";
const char* ntpServer = "pool.ntp.org";

