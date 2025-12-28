/*

showtime.cpp: time functions for Arduino wordclock
mhurk, December 2025

**/

#include <time.h>
#include <FastLED.h>
#include <vector>
#include <algorithm>
#include "showtime.h"
#include "constants.h"  

extern CRGB led[];

void showTime(CRGB clockColor){

time_t now = time(0);
tm *ltm = localtime(&now);
int hour = ltm->tm_hour;
int minute = ltm->tm_min;
  
FastLED.clear();  // clear all pixel data


// All text to be displayed, based on Dutch language wordclock

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

// Minutes between 5-10 - Light "VIJF OVER"
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
// Minutes between 10-15 - Light "TIEN OVER"
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

 // Minutes between 15-20 - Light "KWART OVER"
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

// Minutes between 20-3 - Light "TIEN VOOR HALF"
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

// Minutes between 25-30 - Light "VIJF VOOR HALF"
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

// Minutes between 30-35 - Light "HALF"
  if (minute >= 30 && minute < 35) {
led[39]= clockColor;  // half
led[40]= clockColor;
led[41]= clockColor;
led[42]= clockColor;
 }

// Minutes between 35-40 - Light "VIJF OVER HALF"
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

// Minutes between 40-45 - Light "TIEN OVER HALF"
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

// Minutes between 45-50 - Light "KWART VOOR"
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

// Minutes between 20-3 - Light "TIEN VOOR"
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

// Minutes between 55-60 - Light "VIJF VOOR"
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
 hour = hour + 1;       // because after 20 min the next hour is indicated
}

// Hours
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
