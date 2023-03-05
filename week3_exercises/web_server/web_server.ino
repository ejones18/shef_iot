/* `web_server.ino` script
 *  
 *  Creates a wifi access point and runs a web server.
 *  
 *  Author: Ethan Jones
 *  Date: 2023-02-28
 *
 */

#include <WiFi.h>
#include <WebServer.h>

const int red = 10;
const int yellow = 11;
const int green = 12;

String apSSID;
WebServer webServer(80);
char MAC_ADDRESS[13];

#define startupDBG      true
#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define netDBG          true

//AP functions
void getMAC(char *buf) {
  uint64_t mac = ESP.getEfuseMac();
  char rev[13];
  sprintf(rev, "%04X%08X", (uint16_t) (mac >> 32), (uint32_t) mac);
  for(int i=0, j=11; i<=10; i+=2, j-=2) {
    buf[i] = rev[j-1];
    buf[i+1] = rev[j];
  }
  buf[12] = '\0';
}

void printIPs() {
  if(startupDBG) {
    Serial.print("AP SSID: ");
    Serial.print(apSSID);
    Serial.print("; IP address(es): local=");
    Serial.print(WiFi.localIP());
    Serial.print("; AP=");
    Serial.println(WiFi.softAPIP());
  }
  if(netDBG)
    WiFi.printDiag(Serial);
}

void startAP() {
  getMAC(MAC_ADDRESS);
  apSSID = String("Thing-");
  apSSID.concat(MAC_ADDRESS);
  if(! WiFi.mode(WIFI_AP_STA))
    dln(startupDBG, "failed to set Wifi mode");
  if(! WiFi.softAP(apSSID.c_str(), "dumbpassword"))
    dln(startupDBG, "failed to start soft AP");
  printIPs();
}

//Web server functions
void startWebServer() {
  webServer.on("/", handleRoot);
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  dln(startupDBG, "HTTP server started");
  digitalWrite(yellow, HIGH);
}

void handleNotFound() {
  dbg(netDBG, "URI Not Found: ");
  dln(netDBG, webServer.uri());
  digitalWrite(red, HIGH);
  webServer.send(200, "text/plain", "URI Not Found");
}
void handleRoot() {
  dln(netDBG, "serving page notionally at /");
  String toSend = getPageTop();
  toSend += getPageBody();
  toSend += getPageFooter();
  webServer.send(200, "text/html", toSend);
  digitalWrite(green, HIGH);
}

String getPageTop() {
  return
    "<html><head><title>COM3506 IoT [ID: " + apSSID + "]</title>\n"
    "<meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "\n<style>body{background:#FFF; color: #000; "
    "font-family: sans-serif; font-size: 150%;}</style>\n"
    "</head><body>\n"
  ;
};

String getPageBody() {
  return "<h2>Welcome to Thing!</h2>\n";
}

String getPageFooter() {
  return "\n<p><a href='/'>Home</a>&nbsp;&nbsp;&nbsp;</p></body></html>\n";
}

void setup() {
  Serial.begin(115200);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
  startAP();
  startWebServer();
}

void loop() {
  webServer.handleClient();
}
