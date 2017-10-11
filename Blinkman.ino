#include "config.h"
#include "requesthandler.h"
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//const char *ssid = "";
//const char *password = "";
const char* www_username = "admin";
const char* www_password = "esp8266";

CRGB leds[NUM_LEDS];

void fade() {
  if (g < 256 && !almostfinished) {
    for (int i = -1; i < ledcounter; i++) {
      leds[i] = CRGB(0, g, 0);
    }
    if ((g++ % 32) == 0)
      ledcounter++;
  }
  if (g == 255 && (r < 256)) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(r++, g, 0);
    }
    almostfinished = true;
  }
  if (r == 256 && almostfinished && !finished) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(r - 1, g--, 0);
    }
    if (g < 0) {
      finished = true;
    }
  }
  FastLED.show();
}

void resetAndNull() {
  flipper.detach();
  FastLED.clear(true);
  active = false, almostfinished = false, sem = false, finished = false;
  r = 0, g = 0, ledcounter = 0;
}

void led()
{
  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();
  String state = server.arg("state");
  if (state == "on" && (active
                        != true)) {
    active = true, sem = true;
  }
  else if (state == "off") {
    resetAndNull();
  }
  server.send(200, "text/plain", "Led is now " + state);
}

void buttonPress()
{
  resetAndNull();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  LEDS.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear(true);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }

  attachInterrupt(buttonPin, buttonPress, FALLING);
  Serial.begin(9600);
  WiFi.config(myip, stdgateway, submask);
  WiFi.mode(WIFI_STA);
  WiFi.begin ( ssid, password );

  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }

  MDNS.begin ( "SupportMan" );
  server.on ( "/", handleRoot );
  server.on ( "/led", led );
  server.onNotFound ( handleNotFound );
  server.begin();
}

void loop() {
  server.handleClient();
  if (active && sem) {
    sem = false;
    flipper.attach_ms(50, fade);
  }
}
