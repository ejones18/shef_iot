/*
 * web_client.ino
 * 
 * Send university credentials to a remote server.
 * 
 * Author: Ethan Jones
 * Date: 2023-03-07
 *
*/

#include <WiFiClientSecure.h>
#include <WebServer.h>

WebServer webServer(80); 
WiFiClientSecure com3505Client;

const char* ssid = "<insert_wifi_ssid>";     // your network SSID (name of wifi network)
const char* password = "<insert_wifi_pass>"; // your network password
const char* myEmail = "<uni_email>"; // yr email
const char* com3505Addr = "<server_url>";  // Server URL
const int com3505Port = 9194;  

const char* cert = \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
     "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
     "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
     "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
     "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
     "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
     "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
     "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
     "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
     "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
     "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
     "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
     "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
     "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
     "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
     "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
     "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
     "-----END CERTIFICATE-----\n";

char MAC_ADDRESS[13];

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

// use WiFiClient class to create TCP connection to cloud
bool cloudConnect() {
  com3505Client.setCertificate(cert); // for client verification
  com3505Client.setInsecure();

  Serial.println("\nStarting connection to server...");
  if(com3505Client.connect(com3505Addr, com3505Port)) {
    Serial.println("Connection made!");
    return true;
  }
  else {
    Serial.println("Connected failed!");
    return false;
  }
}

void cloudGet(String url) {
  com3505Client.print(
    String("GET ") + url + " HTTP/1.1\r\n" +
    "Host: " + com3505Addr + "\r\n" + "Connection: close\r\n\r\n"
  );
  unsigned long timeout = millis();
  while(com3505Client.available() == 0) {
    if(millis() - timeout > 5000) {
      Serial.println("Message died");
      cloudStop();
      return;
    }
    Serial.println("Sent message to server!");
  }
}

// close cloud connection
void cloudStop() { com3505Client.stop(); }

// read a line from the cloud client after a request
String cloudRead() { return com3505Client.readStringUntil('\r'); }

// is there more to read from the response?
bool cloudAvailable() { return com3505Client.available(); }

void startWebServer() {
  // register callbacks to handle different paths
  webServer.on("/", handleRoot);
  // 404s...
  webServer.onNotFound(handleNotFound);
}

void handleRoot() {
  String toSend = getPageTop();
  toSend += getPageBody();
  toSend += getPageFooter();
  webServer.send(200, "text/html", toSend);
}
void handleNotFound() {
  webServer.send(200, "text/plain", "URI Not Found");
}


String getPageTop() {
  return
    "<html><head><title>COM3506 IoT</title>\n"
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
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100000);
  startWebServer();
  getMAC(MAC_ADDRESS);
  
  String url;
  url = "/com3505-2023?email=";
  url += myEmail;
  url += "&mac=";
  url += MAC_ADDRESS;

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  if(! cloudConnect()) {
    Serial.println("** OOOPS! no connection to server; restarting in 30 **");
    delay(30000);
    ESP.restart();
  }
  cloudGet(url);
 
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (cloudAvailable()) {
    String c = cloudRead();
    Serial.print(c);
  }
  cloudStop();
}

void loop() {
  webServer.handleClient();
}
