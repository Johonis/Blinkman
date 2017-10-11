#include "wificredentials.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>
#define NUM_LEDS 8

const int buttonPin = D3;
const int DATA_PIN = D1;
bool active, almostfinished, finished, sem = false;
int r = 0, g = 0, ledcounter = 0;

  //http://192.168.178.209/led?state=off
ESP8266WebServer server ( 80 );
IPAddress stdgateway(192, 168, 43, 1);
IPAddress myip(192, 168, 43, 190);
IPAddress submask(255, 255, 255, 0);

Ticker flipper;
