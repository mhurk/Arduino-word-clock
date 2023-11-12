// source time and date via WiFi: https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino
//
// Mayk van den Hurk, May 2022
///////////////////////////////////////////////////


#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <time.h>
#include <ctime>

const char* ssid     = "xxxxx";
const char* password = "xxxxx";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;     //Amsterdam timezone
const int   daylightOffset_sec = 3600;

#define NUM_LEDS 114         
#define LED_PIN 2   
#define CHIPSET WS2812B
#define COLOR_ORDER RGB
#define BRIGHTNESS  25   // pas op. Hoger trekt veel meer stroom en er is geen aparte powersupply.
#define clockColor CRGB(255,0,255)

CRGB led[NUM_LEDS];

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
// display ERROR on LED panel gedurende 10 s
  led[1]= CRGB(255,0,0);
  led[11]= CRGB(255,0,0);
  led[25]= CRGB(255,0,0);
  led[36]= CRGB(255,0,0);
  led[33]= CRGB(255,0,0);
  FastLED.show();
  delay(10000);

    return;
  }

// if time retrieved correctly show green LED's for two secs
  led[110]= CRGB(0,255,0);
  led[111]= CRGB(0,255,0);
  led[112]= CRGB(0,255,0);
  led[113]= CRGB(0,255,0);
  FastLED.show();
  delay(2000);

// and print everything to serial port

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void showTime(){

time_t now = time(0);
tm *ltm = localtime(&now);
int hour = ltm->tm_hour;
int minute = ltm->tm_min;
  
// for debugging:  
//Serial.println(hour);
//Serial.println(minute);

 // FastLED.show(); // Initialize all pixels to ‘off’
FastLED.clear();  // clear all pixel data
//FastLED.show();


// Het is (altijd aan)

led[0]= clockColor;
led[1]= clockColor;
led[2]= clockColor;
led[4]= clockColor;
led[5]= clockColor;


// minuten 0, heel uur
  if (minute >= 0 && minute < 5){
led[101]= clockColor;
led[100]= clockColor;
led[99]= clockColor;
 }

    /* Minutes between 5-10 - Light "VIJF OVER"*/
  if (minute >= 5 && minute < 10){
led[7]= clockColor; // VIJF
led[8]= clockColor;
led[9]= clockColor;
led[10]= clockColor;
led[22]= clockColor; // OVER
led[23]= clockColor;
led[24]= clockColor;
led[25]= clockColor;
 }
    /* Minutes between 10-15 - Light "TIEN OVER"*/
  if (minute >= 10 && minute < 15){
led[18]= clockColor; // TIEN
led[19]= clockColor;
led[20]= clockColor;
led[21]= clockColor;
led[22]= clockColor; // OVER
led[23]= clockColor;
led[24]= clockColor;
led[25]= clockColor;
 }

    /* Minutes between 15-20 - Light "KWART OVER"*/
  if (minute >= 15 && minute < 20) {
led[28]= clockColor; // kwart
led[29]= clockColor;
led[30]= clockColor;
led[31]= clockColor;
led[32]= clockColor;
led[33]= clockColor; // OVER (2e)
led[34]= clockColor;
led[35]= clockColor;
led[36]= clockColor;
 }

    /* Minutes between 20-3 - Light "TIEN VOOR HALF"*/
  if (minute >= 20 && minute < 25) {
led[18]= clockColor; // TIEN
led[19]= clockColor;
led[20]= clockColor;
led[21]= clockColor;
led[11]= clockColor;  // VOOR
led[12]= clockColor;
led[13]= clockColor;
led[14]= clockColor;
led[39]= clockColor;  // half
led[40]= clockColor;
led[41]= clockColor;
led[42]= clockColor;
 }

    /* Minutes between 25-30 - Light "VIJF VOOR HALF"*/
  if (minute >= 25 && minute < 30) {
led[7]= clockColor;   // VIJF
led[8]= clockColor;
led[9]= clockColor;
led[10]= clockColor;
led[11]= clockColor;  // VOOR
led[12]= clockColor;
led[13]= clockColor;
led[14]= clockColor;
led[39]= clockColor;  // half
led[40]= clockColor;
led[41]= clockColor;
led[42]= clockColor;
 }

/* Minutes between 30-35 - Light "HALF"*/
  if (minute >= 30 && minute < 35) {
led[39]= clockColor;  // half
led[40]= clockColor;
led[41]= clockColor;
led[42]= clockColor;
 }

/* Minutes between 35-40 - Light "VIJF OVER HALF"*/
  if (minute >= 35 && minute < 40) {
led[7]= clockColor; // VIJF
led[8]= clockColor;
led[9]= clockColor;
led[10]= clockColor;
led[22]= clockColor; // OVER
led[23]= clockColor;
led[24]= clockColor;
led[25]= clockColor;
led[39]= clockColor;  // half
led[40]= clockColor;
led[41]= clockColor;
led[42]= clockColor;
 }

/* Minutes between 40-45 - Light "TIEN OVER HALF"*/
  if (minute >= 40 && minute < 45) {
led[18]= clockColor; // TIEN
led[19]= clockColor;
led[20]= clockColor;
led[21]= clockColor;
led[22]= clockColor; // OVER
led[23]= clockColor;
led[24]= clockColor;
led[25]= clockColor;
led[39]= clockColor;  // half
led[40]= clockColor;
led[41]= clockColor;
led[42]= clockColor;
 }

/* Minutes between 45-50 - Light "KWART VOOR"*/
  if (minute >= 45 && minute < 50) {
led[28]= clockColor; // kwart
led[29]= clockColor;
led[30]= clockColor;
led[31]= clockColor;
led[32]= clockColor;
led[44]= clockColor;  // VOOR (2e)
led[45]= clockColor;
led[46]= clockColor;
led[47]= clockColor;
 }

/* Minutes between 20-3 - Light "TIEN VOOR"*/
  if (minute >= 50 && minute < 55) {
led[18]= clockColor; // TIEN
led[19]= clockColor;
led[20]= clockColor;
led[21]= clockColor;
led[11]= clockColor;  // VOOR
led[12]= clockColor;
led[13]= clockColor;
led[14]= clockColor;
  }

/* Minutes between 55-60 - Light "VIJF VOOR"*/
  if (minute >= 55 && minute < 60){
led[7]= clockColor; // VIJF
led[8]= clockColor;
led[9]= clockColor;
led[10]= clockColor;
led[11]= clockColor;  // VOOR
led[12]= clockColor;
led[13]= clockColor;
led[14]= clockColor;
 }

// losse minuten na 5

// 1 min na 5
std::vector<int> v = {1,6,11,16,21,26,31,36,41,46,51,56};
 if (std::count(v.begin(), v.end(), minute)) {
        led[110] = clockColor;
    }
// 2 min na 5
std::vector<int> v2 = {2,7,12,17,22,27,32,37,42,47,52,57};
 if (std::count(v2.begin(), v2.end(), minute)) {
        led[110] = clockColor;
        led[111] = clockColor;
    }
// 3 min na 5
std::vector<int> v3 = {3,8,13,18,23,28,33,38,43,48,53,58};
 if (std::count(v3.begin(), v3.end(), minute)) {
        led[110] = clockColor;
        led[111] = clockColor;
        led[112] = clockColor;
    }
// 4 min na 5
std::vector<int> v4 = {4,9,14,19,24,29,34,39,44,49,54,59};
 if (std::count(v4.begin(), v4.end(), minute)) {
        led[110] = clockColor;
        led[111] = clockColor;
        led[112] = clockColor;
        led[113] = clockColor;
    }



if (minute >= 20){
 hour = hour + 1; // want na 20 min wordt het volgende uur genoemd
}


// uren 
if (hour == 1 || hour == 13){
  led[51] = clockColor;
  led[52] = clockColor;
  led[53] = clockColor;
}

if (hour == 2 || hour == 14){
  led[61] = clockColor;
  led[62] = clockColor;
  led[63] = clockColor;
  led[64] = clockColor;
}

if (hour == 3 || hour == 15){
  led[58] = clockColor;
  led[57] = clockColor;
  led[56] = clockColor;
  led[55] = clockColor;
}

if (hour == 4 || hour == 16){
  led[66] = clockColor;
  led[67] = clockColor;
  led[68] = clockColor;
  led[69] = clockColor;
}

if (hour == 5 || hour == 17){
  led[70] = clockColor;
  led[71] = clockColor;
  led[72] = clockColor;
  led[73] = clockColor;
}

if (hour == 6 || hour == 18){
  led[74] = clockColor;
  led[75] = clockColor;
  led[76] = clockColor;
}

if (hour == 7 || hour == 19){
  led[83] = clockColor;
  led[84] = clockColor;
  led[85] = clockColor;
  led[86] = clockColor;
  led[87] = clockColor;
}

if (hour == 8 || hour == 20){
  led[88] = clockColor;
  led[89] = clockColor;
  led[90] = clockColor;
  led[91] = clockColor;
}

if (hour == 9 || hour == 21){
  led[77] = clockColor;
  led[78] = clockColor;
  led[79] = clockColor;
  led[80] = clockColor;
  led[81] = clockColor;
}

if (hour == 10 || hour == 22){
  led[92] = clockColor;
  led[93] = clockColor;
  led[94] = clockColor;
  led[95] = clockColor;
}

if (hour == 11 || hour == 23){
  led[96] = clockColor;
  led[97] = clockColor;
  led[98] = clockColor;
}

if (hour == 12 || hour == 0 || hour == 24){
  led[104] = clockColor;
  led[105] = clockColor;
  led[106] = clockColor;
  led[107] = clockColor;
  led[108] = clockColor;
  led[109] = clockColor;
}

FastLED.show();

}

void intro(){
for (int j = 0 ; j < 100; j++){
int i = rand() % NUM_LEDS-4;   //Random getal tussen 0 en aantal leds met letters
  led[i] = CRGB(255,0,0);  // led kleuren in RGB  
  FastLED.show();
  delay(5);
  led[i] = CRGB(0,0,0);  // led kleuren in RGB 
}
for (int j = 0 ; j < 100; j++){
int i = rand() % NUM_LEDS-4;   //Random getal tussen 0 en aantal leds met letters
  led[i] = CRGB(0,255,0);  // led kleuren in RGB  
  FastLED.show();
  delay(3);
  led[i] = CRGB(0,0,0);  // led kleuren in RGB 
}
for (int j = 0 ; j < 100; j++){
int i = rand() % NUM_LEDS;   //Random getal tussen 0 en aantal leds
  led[i] = CRGB(0,0,255);  // led kleuren in RGB  
  FastLED.show();
  led[i] = CRGB(0,0,0);  // led kleuren in RGB 
}


}

void setup(){
  
FastLED.addLeds<NEOPIXEL, LED_PIN > (led, NUM_LEDS);
FastLED.setBrightness(BRIGHTNESS);
Serial.begin(115200);

// get time from NTP server
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  printLocalTime();

 // disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

intro();


}

void loop(){

 showTime();
 delay(1000);   //update elke seconde

}








