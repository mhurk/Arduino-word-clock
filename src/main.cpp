/* WordClock Project

Some sources: 
time and date via WiFi: https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino
webserver: https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
wineGlass/beerGlass/coctail matrices made with LEDMatrixStudio

Mayk van den Hurk, May 2022
update for wintertime: Oct 2022
    added setTimezone (source: https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/)
    added initTime (also in setup())

 */


#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <time.h>
#include <vector>
using namespace std;

const char* ssid     = "Octo";
const char* password = "61079083Ax$";
WiFiServer server(80);                    // Set web server port number to 80
String header;                            // Variable to store the HTTP request
const char* ntpServer = "pool.ntp.org";   // Time server.
//const long  gmtOffset_sec = 3600;         // Amsterdam timezone
//const int   daylightOffset_sec = 3600;

#define NUM_LEDS 114         
#define LED_PIN 2   
#define CHIPSET WS2812B
#define COLOR_ORDER RGB
#define BRIGHTNESS  25   // pas op. Hoger trekt veel meer stroom en er is geen aparte powersupply. Alleen gebruikt bij intro, later aangepast aan licht
CRGB ledColor = CRGB(255,0,255);   // de standaard(begin)kleur
//#define PERIOD (2*60*1000L)       //timer voor checken timezone
//unsigned long target_time = 0L ;   //ook voor de timer


// voor auto-brightness
double minBrightness = 5;      // min brightness for LEDs
double maxBrightness = 20;     // maximum brightness for LEDs
double minAnalogvalue = 150;   // min value from light sensor
double maxAnalogvalue = 4095;  // max value from light sensor
const double slope = (maxBrightness-minBrightness) / (maxAnalogvalue-minAnalogvalue);  // berekening slope (lin regression voor twee punten), m = (y2-y1) / (x2-x1) 
const double intercept = minBrightness - (slope * minAnalogvalue);                     // b = y1 - (m*x1)


// voor network server: 
unsigned long currentTime = millis();   // Current time
unsigned long previousTime = 0;         // Previous time
const long timeoutTime = 2000;         // Define timeout time in milliseconds (example: 2000ms = 2s)

//voor regelmatig checken timezone
//unsigned long currentTimeCheck = millis();   // Current time
unsigned long StartTimeCheck = 0;         // Previous time
const long WaitTimeCheck = (1000*60*5);         // Define timeout time in milliseconds (example: 2000ms = 2s)


CRGB led[NUM_LEDS];           // Deze moet als laatste gedefineerd worden anders gaat het mis met die brightness variabelen. Waarom?????


void setTimezone(String timezone){
  Serial.printf("  Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone){
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "europe.pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo)){
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

void printLocalTime(){
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
// display ERROR on LED panel gedurende 5 s
  led[1]= CRGB(255,0,0);
  led[11]= CRGB(255,0,0);
  led[25]= CRGB(255,0,0);
  led[36]= CRGB(255,0,0);
  led[33]= CRGB(255,0,0);
  FastLED.show();
  delay(5000);

    return;
  }

// if time retrieved correctly show green LEDs for one secs
  led[110]= CRGB(0,255,0);
  led[111]= CRGB(0,255,0);
  led[112]= CRGB(0,255,0);
  led[113]= CRGB(0,255,0);
  FastLED.show();
  delay(1000);

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

void showTime(CRGB clockColor){

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
  led[i] = CRGB(255,0,0);      // led kleuren in RGB  
  FastLED.show();
  delay(3);
  led[i] = CRGB(0,0,0);        // led kleuren in RGB 
}
for (int j = 0 ; j < 100; j++){
int i = rand() % NUM_LEDS-4;   //Random getal tussen 0 en aantal leds met letters
  led[i] = CRGB(0,255,0);      // led kleuren in RGB  
  FastLED.show();
  delay(2);
  led[i] = CRGB(0,0,0);        // led kleuren in RGB 
}
for (int j = 0 ; j < 100; j++){
int i = rand() % NUM_LEDS;     //Random getal tussen 0 en aantal leds
  led[i] = CRGB(0,0,255);      // led kleuren in RGB  
  FastLED.show();
  led[i] = CRGB(0,0,0);        // led kleuren in RGB 
}

}

void test_all(){

for (int i = 0 ; i < NUM_LEDS; i++){
  led[i] = CRGB(0,255,0);  
  FastLED.setBrightness(5);
  FastLED.show();
    
}
}

void wineGlass(){
 
FastLED.clear();

int pixels = 110;
int wineDelay = 400;       // delay tussen frames

const long ledarray0[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray1[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray2[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray3[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray4[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};
          

for (int i = 0; i < pixels; i ++){
led[i] = ledarray0[i];
}
FastLED.show();
delay(wineDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray1[i];
}
FastLED.show();
delay(wineDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray2[i];
}
FastLED.show();
delay(wineDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray3[i];
}
FastLED.show();
delay(wineDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray4[i];
}
FastLED.show();
delay(wineDelay);

} 

void beerGlass(){

FastLED.clear();
int pixels = 110;

long ledarray[] = {
                   0x00000000, 0x00808080, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00808080, 0x00000000, 0x00000000, 0x00000000, 
                   0x00000000, 0x00000000, 0x00000000, 0x00808080, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00808080, 0x00000000, 
                   0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00808080, 0x00808080, 0x00000000, 
                   0x00808080, 0x00000000, 0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00000000, 
                   0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00000000, 0x00000000, 0x00808080, 
                   0x00808080, 0x00000000, 0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00000000, 
                   0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00808080, 0x00808080, 0x00000000, 
                   0x00000000, 0x00000000, 0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00000000, 
                   0x00000000, 0x00808080, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00FFFF44, 0x00808080, 0x00000000, 0x00000000, 0x00000000, 
                   0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00808080, 0x00808080, 0x00808080, 0x00808080, 0x00808080, 0x00000000, 0x00000000, 
                   };

for (int i = 0; i < pixels; i ++){
led[i] = ledarray[i];
}
FastLED.show();
delay(10000);

}

void strobo(){

FastLED.clear();
int pixels = 110;

for (int j = 0; j < 30; j++){

    for (int i = 0; i < pixels; i ++){
    led[i] = CRGB(255,255,255);
    }
    FastLED.show();

  delay(rand() % 100 + 50);   //random delay in the range 50 to 150 ms (on time)
  FastLED.clear();
  FastLED.show();
  delay(rand() % 100 + 100);  // off time
  }
}

void coctailGlass(){

FastLED.clear();
int pixels = 110;
int coctailDelay = 500;       // delay tussen frames

const long ledarray0[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray1[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray2[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x000080FF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray3[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 
				0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x000080FF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

const long ledarray4[] PROGMEM = {
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FF8800, 0x00000000, 
        0x00FF8800, 0x00FFFFFF, 0x00FF8800, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x00FFFFFF, 0x00000000, 				
				0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x000080FF, 0x00FFFFFF, 0x00FF8800, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x000080FF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x000080FF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
				0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 
};

          

for (int i = 0; i < pixels; i ++){
led[i] = ledarray0[i];
}
FastLED.show();
delay(coctailDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray1[i];
}
FastLED.show();
delay(coctailDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray2[i];
}
FastLED.show();
delay(coctailDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray3[i];
}
FastLED.show();
delay(coctailDelay);

for (int i = 0; i < pixels; i ++){
led[i] = ledarray4[i];
}
FastLED.show();
delay(coctailDelay);

}

void setup(){
  
FastLED.addLeds<NEOPIXEL, LED_PIN > (led, NUM_LEDS);
FastLED.setBrightness(BRIGHTNESS);                      //set initial Brightness
Serial.begin(115200);

// get time from NTP server
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();                     // start webserver
  
  // Init and get the time
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Amsterdam

  printLocalTime();

 // disconnect WiFi as it's no longer needed
 // WiFi.disconnect(true);
 // WiFi.mode(WIFI_OFF);

  intro();

}

void loop(){

//unsigned long StartTimeCheck = 0;         // Previous time
//const long WaitTimecheck = (1000*60*5);         // Define timeout time in milliseconds (example: 2000ms = 2s)


//Check timezone every 5 minutes
   if(millis() >= StartTimeCheck + WaitTimeCheck){
        StartTimeCheck += WaitTimeCheck;
        Serial.println("5 minuten zijn voorbij, tijd om de timezone te checken");
        printLocalTime();
        initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Amsterdam
    }



showTime(ledColor);
 delay(200);   //update elke 200 ms

// aanpassen brightness. Lager bij een donkerdere kamer, hoger als het lichter is.
int analogValue = analogRead(A0);  //pin VP
//Serial.print("Analog reading: ");   // for debugging
//Serial.println(analogValue);        // the raw analog reading
int newBrightness = analogValue * slope + intercept;   //int want dan wordt het meteen mooi een rond getal
/*   Serial.print("instelling voor Brightness: ");
  Serial.println(newBrightness);   // the raw analog reading */
FastLED.setBrightness(newBrightness); 
int brightPerc = (analogValue / maxAnalogvalue)*100;
/*             Serial.print("brightness (%): ");           // for debugging
            Serial.println(String(brightPerc));

            Serial.print("newbrightness: ");
            Serial.println(String(newBrightness)); */

String wait_anim = "<p style=\"text-align:center;\"><img src=\"http://192.168.5.9/wordpress/wp-content/uploads/wine_wait.gif\"></p>";
String wait_end = "<meta http-equiv=\"Refresh\" content=\"0;http://wordclock.octo\">";

// webserver

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // show different glasses and LED colors
             
            if (header.indexOf("GET /cocktail") >= 0) {
              Serial.println("coctail");
              client.println(wait_anim);
              for (int i = 0; i < 5; i++){
                   coctailGlass(); 
                   }
              client.println(wait_end);
            }
            
            else if (header.indexOf("GET /wine") >= 0) {
              Serial.println("wine!");
              client.println(wait_anim);
              for (int i = 0; i < 5; i++){
                  wineGlass();
                  }
              client.println(wait_end);
            } 
            
            else if (header.indexOf("GET /beer") >= 0) {
              Serial.println("time for a beer!!");
              client.println(wait_anim);
              beerGlass();  
              client.println(wait_end);          
            } 
            
            else if (header.indexOf("GET /strobo") >= 0) {
              client.println(wait_anim);
              Serial.println("strobo!!!!");
              strobo();  
              client.println(wait_end);          
            } 

            else if (header.indexOf("GET /test") >= 0) {
              Serial.println("testmode");
              test_all();
              delay(10000);  //all leds on in green, 10s
            } 

            // verschillende kleuren
            else if (header.indexOf("GET /standaard") >= 0) {
              Serial.println("standaard kleur");
              ledColor = CRGB(255,0,255);        
            }

            else if (header.indexOf("GET /wit") >= 0) {
              Serial.println("wit");
              ledColor = CRGB(255,255,255);          
            }

            else if (header.indexOf("GET /rood") >= 0) {
              Serial.println("rood");
              ledColor = CRGB(255,0,0) ;           
            }

            else if (header.indexOf("GET /groen") >= 0) {
              Serial.println("groen");
              ledColor = CRGB(0,255,0);            
            }

            else if (header.indexOf("GET /blauw") >= 0) {
              Serial.println("blauw");
              ledColor = CRGB(0,0,255);          
            }

            else if (header.indexOf("GET /lichtcyaan") >= 0) {
              Serial.println("licht cyaan");
              ledColor = CRGB(24,87,100);          
            }

            else if (header.indexOf("GET /oranje") >= 0) {
              Serial.println("oranje");
              ledColor = CRGB(255, 127, 0);          
            }

            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button0 { vertical-align:middle; background-color: #000000; border: none; color: white; padding: 12px 40px; border-radius: 8px;");
            client.println("text-decoration: none; font-size: 20px; margin: 2px; }");

           //buttons voor andere kleuren (button group)
            client.println(".btn-group button {border: 4px solid white; color: white; padding: 15px 15px; cursor: pointer;  float: center;  border-radius: 8px;}");
            client.println(".btn-group:after {  content: \"\";  clear: both;  display: table;}");
            client.println(".btn-group button:not(:last-child) { border-right: none; }");
            client.println(".btn-group button:hover {background-color: #3e8e41;}");  // Add a background color on hover
          //  client.println(".center {display: block; margin-left: auto; margin-right: auto; width: 20%;}");
            client.println("</style></head>");


            // Web Page Heading
            client.println("<body><h1>WordClock</h1>");
                          
            // display buttons for glasses
            client.println("<p><img align=\"middle\" src=\"http://192.168.5.9/wordpress/wp-content/uploads/cocktail_64.png\"><a href=\"/cocktail\"><button class=\"button0\">Cocktail</button></a></p>");
            client.println("<p><img align=\"middle\" src=\"http://192.168.5.9/wordpress/wp-content/uploads/wine_64.png\"><a href=\"/wine\"><button class=\"button0\">Wijn</button></a></p>");
            client.println("<p><img align=\"middle\" src=\"http://192.168.5.9/wordpress/wp-content/uploads/beer_mug_64.png\"><a href=\"/beer\"><button class=\"button0\">Bier</button></a></p>");
            client.println("<br>");

            //display buttons for colors
            client.println("<div class=\"btn-group\" style=\"width:100%\" align = center>");
            client.println("<a href=\"/standaard\"><button style=\"width:20 px; background-color: #FF00FF;\"></button></a>");
            client.println("<a href=\"/wit\"><button style=\"width:16 px; background-color: #FFFFFF; border: 2px solid black;\"></button></a>");
            client.println("<a href=\"/rood\"><button style=\"width:20 px; background-color: #FF0000;\"></button></a>");
            client.println("<a href=\"/groen\"><button style=\"width:20 px; background-color: #00FF00;\"></button></a>");
            client.println("<a href=\"/blauw\"><button style=\"width:20 px; background-color: #0000FF;\"></button></a>");
            client.println("<a href=\"/lichtcyaan\"><button style=\"width:20 px; background-color: #3cdfff;\"></button></a>");
            client.println("<a href=\"/oranje\"><button style=\"width:20 px; background-color: #FF7F00;\"></button></a></div>");

            //display image
            client.println("<br><p><img src=\"http://192.168.5.9/wordpress/wp-content/uploads/lucht.webp\" style = \"width:75%\"></p>");
                      
            //display brightness
            client.println("<p>Brightness: " + String(brightPerc) + "%</p>"); 
            client.println("<small>Web interface version 0.4</small>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
  }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    //Serial.println("Client disconnected.");
    //Serial.println("");



}








