// constants.h
#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include <WebServer.h>

#pragma once

// LED settings
#define NUM_LEDS 114         
#define LED_PIN 2   
#define CHIPSET WS2812B
#define COLOR_ORDER RGB
#define BRIGHTNESS  25                      // Be careful!. Higher brightness draws higher current. This runs on just a wall plug, no external power supply. Only used at intro, later adapted to room light

// used for auto-brightness
// declarations (defined in src/constants.cpp)
extern double minBrightness;                 // min brightness for LEDs
extern double maxBrightness;                 // maximum brightness for LEDs
extern double minAnalogvalue;                // min value from light sensor
extern double maxAnalogvalue;                // max value from light sensor
extern double slope;                         // slope for brightness calculation
extern double intercept;                     // intercept for brightness calculation

// network server: 
// network server variables (defined in src/constants.cpp)
extern unsigned long currentTime;            // Current time
extern unsigned long previousTime;           // Previous time
extern const long timeoutTime;               // Define timeout time in milliseconds (example: 2000ms = 2s)

// Time check settings (defined in src/constants.cpp)
extern unsigned long StartTimeCheck;         // Previous time
extern const long WaitTimeCheck;             // Define timeout time in milliseconds 

// WiFi credentials (defined in src/constants.cpp)
extern const char* ssid;
extern const char* password;
extern WebServer server;                     // Set web server port number to 80
extern const char* ntpServer;                // Time server.
const long  gmtOffset_sec = 3600;            // Amsterdam timezone

// Animation settings
extern int animRepeats;                      // number of times to repeat animations