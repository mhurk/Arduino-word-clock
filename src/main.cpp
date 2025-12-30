/* WordClock Project

Some sources: 
time and date via WiFi: https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino
webserver: https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
wineGlass/beerGlass/coctail matrices made with LEDMatrixStudio

mhurk, May 2022
update for wintertime: Oct 2022
    added setTimezone (source: https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/)
    added initTime (also in setup())

Update December 2025:
- refactor of time functions to showtime.h and animations to animations.h
- add contants.h for constants like brightness and hardware settings
- change file locations for webinterface to local filesystem LittleFS
- change webserver type from TCP socket–based HTTP (manual socket streaming implemented in main.cpp) to Arduino WebServer library (synchronous)
  with server.streamFile() / server.serveStatic() to serve files from LittleFS. This was needed to serve images direct from filesystem. 

 */


#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <time.h>
#include "animations.h"
#include "showtime.h"
#include "constants.h"
using namespace std;

CRGB ledColor = CRGB(255,0,255);              // default color magenta
CRGB led[NUM_LEDS];                           // Define last otherwise brightness won't work in functions 

// Forward declarations for WebServer handlers and helpers
String getContentType(const String& filename);
enum AnimationType { ANIM_NONE=0, ANIM_COCKTAIL, ANIM_WINE, ANIM_BEER, ANIM_TEST_ALL, ANIM_STROBO };
volatile AnimationType pendingAnim = ANIM_NONE;
volatile bool animActive = false;
volatile unsigned long pendingAnimRequestTime = 0;

const unsigned long ANIM_START_DELAY_MS = 300; // ms to wait before starting animation so wait-page loads
void handleRoot();
void handleAnimCocktail();
void handleAnimWine();
void handleAnimBeer();
void handleAnimStrobo();
void handleAnimTestAll();
void handleAnimStatus();
void handleColorStand();
void handleColorWit();
void handleColorRood();
void handleColorGroen();
void handleColorBlauw();
void handleColorCyaan();
void handleColorOranje();
void handleNotFound();
void handleFavicon();

void setTimezone(String timezone){
  Serial.printf("  Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);            //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
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


void test_all(){

for (int i = 0 ; i < NUM_LEDS; i++){
  led[i] = CRGB(0,255,0);  
  FastLED.setBrightness(5);
  FastLED.show();
  delay(2000);
    
}
}

void setup(){
  
FastLED.addLeds<NEOPIXEL, LED_PIN > (led, NUM_LEDS);
FastLED.setBrightness(BRIGHTNESS);                      //set initial brightness
Serial.begin(115200);

// get time from NTP server
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Initialize LittleFS (required to serve images from filesystem)
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed, attempting format...");
    if (!LittleFS.begin(true)) {
      Serial.println("LittleFS format failed");
    } else {
      Serial.println("LittleFS formatted and mounted");
    }
  } else {
    Serial.println("LittleFS mounted");
  }

    // Register WebServer routes (handlers defined below)
    server.on("/", handleRoot);
    server.on("/cocktail", handleAnimCocktail);
    server.on("/wine", handleAnimWine);
    server.on("/beer", handleAnimBeer);
    server.on("/strobo", handleAnimStrobo);
    server.on("/test", handleAnimTestAll);
    server.on("/standaard", handleColorStand);
    server.on("/wit", handleColorWit);
    server.on("/rood", handleColorRood);
    server.on("/groen", handleColorGroen);
    server.on("/blauw", handleColorBlauw);
    server.on("/lichtcyaan", handleColorCyaan);
    server.on("/oranje", handleColorOranje);
    server.on("/favicon.ico", HTTP_ANY, handleFavicon);
    server.on("/favicon-32x32.png", HTTP_ANY, handleFavicon);
    server.on("/favicon-16x16.png", HTTP_ANY, handleFavicon);
    server.on("/apple-touch-icon.png", HTTP_ANY, handleFavicon);
    server.on("/manifest.json", HTTP_ANY, handleFavicon);
    server.on("/robots.txt", HTTP_ANY, handleFavicon);
    server.on("/anim_status", handleAnimStatus);
    server.serveStatic("/images", LittleFS, "/images");
    server.onNotFound(handleNotFound);

    server.begin();                           // start webserver
  
  // Init and get the time
  initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Amsterdam. Initial setting, omitting this results in waiting for the check loop.
    printLocalTime();

  intro();   // intro animation

}

void loop(){

//Check timezone every 5 minutes
   if(millis() >= StartTimeCheck + WaitTimeCheck){
        StartTimeCheck += WaitTimeCheck;
        Serial.println("5 minutes have passed, rechecking timezone and time");
        printLocalTime();
        initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Amsterdam
    }


showTime(ledColor);
 delay(200);   //update elke 200 ms

// aanpassen brightness. Lager bij een donkerdere kamer, hoger als het lichter is.
int analogValue = analogRead(A0);  //pin VP, read anaog waarde van light sensor
int newBrightness = analogValue * slope + intercept;   //int is good enough here
FastLED.setBrightness(newBrightness); 
int brightPerc = (analogValue / maxAnalogvalue)*100;

  // Handle incoming HTTP requests
  server.handleClient();

  // Process any queued animation requested via the web UI
  if (pendingAnim != ANIM_NONE) {
    // wait a short time after the request so the browser can load the wait page and its assets
    if (pendingAnimRequestTime != 0 && (millis() - pendingAnimRequestTime) < ANIM_START_DELAY_MS) {
      // still waiting; allow server.handleClient() to serve assets
    } else {
      // grab and clear the pending animation
      AnimationType anim = pendingAnim;
      pendingAnim = ANIM_NONE;
      pendingAnimRequestTime = 0;
      // mark active
      animActive = true;
      switch (anim) {
      case ANIM_COCKTAIL:
        for (int r = 0; r < animRepeats; ++r) {
          coctailGlass();
        }
        break;
      case ANIM_WINE:
        for (int r = 0; r < animRepeats; ++r) {
          wineGlass();
        }
        break;
      case ANIM_BEER:
        for (int r = 0; r < animRepeats; ++r) {
          beerGlass();
        }
        break;
      case ANIM_TEST_ALL:
        for (int r = 0; r < animRepeats; ++r) {
          test_all();
        }
        break;
      case ANIM_STROBO:
        for (int r = 0; r < 2 ; ++r) {     // strobo always runs 2 times, independent of animRepeats
          strobo();
        }
        break;
      default:
        break;
    }
    animActive = false;
  }
  }

}

// -- replaced low-level socket handling with WebServer endpoints --
// Build content-type from extension
String getContentType(const String& filename) {
  if (filename.endsWith(".htm") || filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".js")) return "application/javascript";
  if (filename.endsWith(".png")) return "image/png";
  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
  if (filename.endsWith(".gif")) return "image/gif";
  if (filename.endsWith(".webp")) return "image/webp";
  return "application/octet-stream";
}

// Handlers
void handleRoot() {
  String page;
  page += "<!DOCTYPE html><html>";
  page += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  page += "<link rel=\"icon\" href=\"data:,\">";
  page += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  page += ".button0 { vertical-align:middle; background-color: #000000; border: none; color: white; padding: 12px 40px; border-radius: 8px;";
  page += "text-decoration: none; font-size: 20px; margin: 2px; }";
  page += ".btn-group button {border: 4px solid white; color: white; padding: 15px 15px; cursor: pointer;  float: center;  border-radius: 8px;}";
  page += ".btn-group:after {  content: \"\";  clear: both;  display: table;}";
  page += ".btn-group button:not(:last-child) { border-right: none; }";
  page += ".btn-group button:hover {background-color: #3e8e41;}";
  page += "</style></head>";
  page += "<body><h1>WordClock</h1>";
  page += "<p><img align=\"middle\" src=\"/images/cocktail_64.png\"><a href=\"/cocktail\"><button class=\"button0\">Cocktail</button></a></p>";
  page += "<p><img align=\"middle\" src=\"/images/wine_64.png\"><a href=\"/wine\"><button class=\"button0\">Wine</button></a></p>";
  page += "<p><img align=\"middle\" src=\"/images/beer_mug_64.png\"><a href=\"/beer\"><button class=\"button0\">Beer</button></a></p>";
  page += "<br>";
  page += "<div class=\"btn-group\" style=\"width:100%\" align = center>";
  page += "<a href=\"/standaard\"><button style=\"width:20 px; background-color: #FF00FF;\"></button></a>";
  page += "<a href=\"/wit\"><button style=\"width:16 px; background-color: #FFFFFF; border: 2px solid black;\"></button></a>";
  page += "<a href=\"/rood\"><button style=\"width:20 px; background-color: #FF0000;\"></button></a>";
  page += "<a href=\"/groen\"><button style=\"width:20 px; background-color: #00FF00;\"></button></a>";
  page += "<a href=\"/blauw\"><button style=\"width:20 px; background-color: #0000FF;\"></button></a>";
  page += "<a href=\"/lichtcyaan\"><button style=\"width:20 px; background-color: #3cdfff;\"></button></a>";
  page += "<a href=\"/oranje\"><button style=\"width:20 px; background-color: #FF7F00;\"></button></a></div>";
  page += "<br><p><img src=\"/images/lucht.webp\" style = \"width:70%\"></p>";
  page += "<p>Brightness: " + String((int)map(analogRead(A0), 0, 4095, 0, 100)) + "%</p>";
  page += "<small>Web interface version 1.0</small><br>";
  page += "<small>IP address: " + WiFi.localIP().toString() + "</small><br>";
  page += "<small>mhurk 2022-2025</small>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

// Return a waiting page that polls /anim_status; when done it shows wait_end then redirects
String buildWaitPage(){
  String page;
  page += "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  page += "<meta charset=\"utf-8\"><title>Waiting</title></head><body>";
  page += "<p style=\"text-align:center;\"><img src=\"/images/wait.gif\" style=\"max-width:90%\"></p>";
  page += "<script>function poll(){fetch('/anim_status').then(r=>r.text()).then(t=>{if(t=='done'){document.body.innerHTML='<meta http-equiv=\"refresh\" content=\"1;url=/\">';}else{setTimeout(poll,500);}}).catch(()=>setTimeout(poll,1000));}setTimeout(poll,500);</script>";
  page += "</body></html>";
  return page;
}

void handleAnimCocktail() { pendingAnim = ANIM_COCKTAIL; pendingAnimRequestTime = millis(); server.send(200, "text/html", buildWaitPage()); }
void handleAnimWine()    { pendingAnim = ANIM_WINE; pendingAnimRequestTime = millis(); server.send(200, "text/html", buildWaitPage()); }
void handleAnimBeer()    { pendingAnim = ANIM_BEER; pendingAnimRequestTime = millis(); server.send(200, "text/html", buildWaitPage()); }
void handleAnimStrobo()  { pendingAnim = ANIM_STROBO; pendingAnimRequestTime = millis(); server.send(200, "text/html", buildWaitPage()); }
void handleAnimTestAll() { pendingAnim = ANIM_TEST_ALL; pendingAnimRequestTime = millis(); server.send(200, "text/html", buildWaitPage()); }

void handleAnimStatus(){
  if(animActive || pendingAnim != ANIM_NONE) server.send(200, "text/plain", "running");
  else server.send(200, "text/plain", "done");
}

void handleColorStand() { ledColor = CRGB(255,0,255); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }
void handleColorWit()   { ledColor = CRGB(255,255,255); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }
void handleColorRood()  { ledColor = CRGB(255,0,0); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }
void handleColorGroen() { ledColor = CRGB(0,255,0); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }
void handleColorBlauw() { ledColor = CRGB(0,0,255); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }
void handleColorCyaan() { ledColor = CRGB(24,87,100); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }
void handleColorOranje(){ ledColor = CRGB(255,127,0); server.sendHeader("Location","/",true); server.send(302, "text/plain", ""); }

// NotFound handler: serve files from LittleFS under /images/
void handleNotFound(){
  String uri = server.uri();
  if (uri.startsWith("/images/")) {
    // Only check absolute paths under /images/ — LittleFS expects leading '/'
    bool exists = LittleFS.exists(uri);
    if (!exists) {
      server.send(404, "text/plain", "Not found");
      return;
    }
    File f = LittleFS.open(uri, "r");
    if (!f) {
      server.send(404, "text/plain", "Not found");
      return;
    }
    String ct = getContentType(uri);
    server.sendHeader("Connection","close");
    server.streamFile(f, ct);
    f.close();
    return;
  }
  // For other non-image requests (browser probes), return No Content to silence logs
  server.send(204, "text/plain", "");
}

// Handle favicon/manifest/robots requests: return no content to silence browser probes
void handleFavicon(){
  server.send(204, "text/plain", "");
}

// -- end of WebServer handlers --

