/*
 * wifi_scanner.ino
 * 
 * List all available APs and create a form to connect.
 * 
 * Author: Ethan Jones
 * Credit: Hamish Cunningham
 * Date: 2023-03-08
 *
*/

#include <WebServer.h>

#define ALEN(a) ((int) (sizeof(a) / sizeof(a[0]))) // only in definition scope!
#define GET_HTML(strout, boiler, repls) \
  getHtml(strout, boiler, ALEN(boiler), repls, ALEN(repls));

extern const char *boiler[];
typedef struct { int position; const char *replacement; } replacement_t;

WebServer webServer(80);
char MAC_ADDRESS[13];
String apSSID;

void getHtml(String& html, const char *[], int, replacement_t [], int);

const char *templatePage[] = {
  "<html><head><title>",                                                //  0
  "default title",                                                      //  1
  "</title>\n",                                                         //  2
  "<meta charset='utf-8'>",                                             //  3
  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
  "<style>body{background:#FFF; color: #000; font-family: sans-serif;", //  4
  "font-size: 150%;}</style>\n",                                        //  5
  "</head><body>\n",                                                    //  6
  "<h2>Welcome to Thing!</h2>\n",                                       //  7
  "<!-- page payload goes here... -->\n",                               //  8
  "<!-- ...and/or here... -->\n",                                       //  9
  "\n<p><a href='/'>Home</a>&nbsp;&nbsp;&nbsp;</p>\n",                  // 10
  "</body></html>\n\n",                                                 // 11
};

String ip2str(IPAddress address) { // utility for printing IP addresses
  return
    String(address[0]) + "." + String(address[1]) + "." +
    String(address[2]) + "." + String(address[3]);
}

void getHtml( // turn array of strings & set of replacements into a String
  String& html, const char *templatePage[], int boilerLen,
  replacement_t repls[], int replsLen
) {
  for(int i = 0, j = 0; i < boilerLen; i++) {
    if(j < replsLen && repls[j].position == i)
      html.concat(repls[j++].replacement);
    else
      html.concat(templatePage[i]);
  }
}

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

void initWebServer() { // changed naming conventions to avoid clash with Ex06
  // register callbacks to handle different paths
  webServer.on("/", hndlRoot);              // slash
  webServer.onNotFound(hndlNotFound);       // 404s...
  webServer.on("/generate_204", hndlRoot);  // Android captive portal support
  webServer.on("/L0", hndlRoot);            // TODO is this...
  webServer.on("/L2", hndlRoot);            // ...IoS captive portal...
  webServer.on("/ALL", hndlRoot);           // ...stuff?
  webServer.on("/wifi", hndlWifi);          // page for choosing an AP
  webServer.on("/wifichz", hndlWifichz);    // landing page for AP form submit
  webServer.on("/status", hndlStatus);      // status check, e.g. IP address
  webServer.begin();
}

// webserver handler callbacks
void hndlNotFound() {
  webServer.send(200, "text/plain", "URI Not Found");
}

void hndlRoot() {
  Serial.print("Serving page at /");
  replacement_t repls[] = { // the elements to replace in the boilerplate
    {  1, apSSID.c_str() },
    {  8, "" },
    {  9, "<p>Choose a <a href=\"wifi\">wifi access point</a>.</p>" },
    { 10, "<p>Check <a href='/status'>wifi status</a>.</p>" },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls); // GET_HTML sneakily added to Ex07
  webServer.send(200, "text/html", htmlPage);
}

void hndlWifi() {
  Serial.print("Serving page at /wifi");
  String form = ""; // a form for choosing an access point and entering key
  apListForm(form);
  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 7, "<h2>Network configuration</h2>\n" },
    { 8, "" },
    { 9, form.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls); // GET_HTML sneakily added to Ex07
  webServer.send(200, "text/html", htmlPage);
}

void hndlWifichz() {
  Serial.print("Serving page at /wifichz");
  String title = "<h2>Joining wifi network...</h2>";
  String message = "<p>Check <a href='/status'>wifi status</a>.</p>";
  String ssid = "";
  String key = "";
  for(uint8_t i = 0; i < webServer.args(); i++ ) {
    if(webServer.argName(i) == "ssid")
      ssid = webServer.arg(i);
    else if(webServer.argName(i) == "key")
      key = webServer.arg(i);
  }
  if(ssid == "") {
    message = "<h2>Ooops, no SSID...?</h2>\n<p>Looks like a bug :-(</p>";
  } else {
    char ssidchars[ssid.length()+1];
    char keychars[key.length()+1];
    ssid.toCharArray(ssidchars, ssid.length()+1);
    key.toCharArray(keychars, key.length()+1);
    WiFi.begin(ssidchars, keychars);
  }
  replacement_t repls[] = { // the elements to replace in the template
    { 1, apSSID.c_str() },
    { 7, title.c_str() },
    { 8, "" },
    { 9, message.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);
  webServer.send(200, "text/html", htmlPage);
}

void hndlStatus() { // UI for checking connectivity etc.
  Serial.print("Serving page at /status");
  String s = "";
  s += "<ul>\n";
  s += "\n<li>SSID: ";
  s += WiFi.SSID();
  s += "</li>";
  s += "\n<li>Status: ";
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      s += "WL_IDLE_STATUS</li>"; break;
    case WL_NO_SSID_AVAIL:
      s += "WL_NO_SSID_AVAIL</li>"; break;
    case WL_SCAN_COMPLETED:
      s += "WL_SCAN_COMPLETED</li>"; break;
    case WL_CONNECTED:
      s += "WL_CONNECTED</li>"; break;
    case WL_CONNECT_FAILED:
      s += "WL_CONNECT_FAILED</li>"; break;
    case WL_CONNECTION_LOST:
      s += "WL_CONNECTION_LOST</li>"; break;
    case WL_DISCONNECTED:
      s += "WL_DISCONNECTED</li>"; break;
    default:
      s += "unknown</li>";
  }
  s += "\n<li>Local IP: ";     s += ip2str(WiFi.localIP());
  s += "</li>\n";
  s += "\n<li>Soft AP IP: ";   s += ip2str(WiFi.softAPIP());
  s += "</li>\n";
  s += "\n<li>AP SSID name: "; s += apSSID;
  s += "</li>\n";
  s += "</ul></p>";
  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 7, "<h2>Status</h2>\n" },
    { 8, "" },
    { 9, s.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);
  webServer.send(200, "text/html", htmlPage);
}

void apListForm(String& f) { // utility to create a form for choosing AP
  const char *checked = " checked";
  int n = WiFi.scanNetworks();
  Serial.print("Scan completed ");
  if(n == 0) {
    Serial.print("No networks found");
    f += "No wifi access points found :-( ";
    f += "<a href='/'>Back</a><br/><a href='/wifi'>Try again?</a></p>\n";
  } else {
    Serial.print("Networks found!");
    f += "<p>Wifi access points available:</p>\n"
         "<p><form method='POST' action='wifichz'> ";
    for(int i = 0; i < n; ++i) {
      f.concat("<input type='radio' name='ssid' value='");
      f.concat(WiFi.SSID(i));
      f.concat("'");
      f.concat(checked);
      f.concat(">");
      f.concat(WiFi.SSID(i));
      f.concat(" (");
      f.concat(WiFi.RSSI(i));
      f.concat(" dBm)");
      f.concat("<br/>\n");
      checked = "";
    }
    f += "<br/>Pass key: <input type='textarea' name='key'><br/><br/> ";
    f += "<input type='submit' value='Submit'></form></p>";
  }
}

void startAP() {
  apSSID = String("Thing-");
  apSSID.concat(MAC_ADDRESS);
  if(! WiFi.mode(WIFI_AP_STA))
    Serial.print("Failed to set Wifi mode");
  if(! WiFi.softAP(apSSID.c_str(), "dumbpassword"))
    Serial.print("Failed to start soft AP");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // initialise the serial line
  delay(100000);
  getMAC(MAC_ADDRESS);  // store the MAC address as a chip identifier
  apSSID = String("Thing-");
  apSSID.concat(MAC_ADDRESS);
  startAP();            // fire up the AP...
  initWebServer();      // ...and the web server
  WiFi.begin();         // for when creds already stored by board; if none:         
}

void loop() {
  // put your main code here, to run repeatedly:
  webServer.handleClient();
}
