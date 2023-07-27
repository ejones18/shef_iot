/**
 * @file car_detection.ino
 * @brief Central sketch for the car detection web application. Contains the raw HTML, web server root handles and set-up functions.
 *
 * @author Ethan Jones
 * @date 2023-04-17
 */

#include <Arduino.h>  // Standard Arduino library
#include <WiFi.h>  // WiFi library
#include <esp_timer.h>  // ESP timer library
#include <img_converters.h>  // Image conversion library
#include <soc/soc.h>  // SoC library
#include <soc/rtc_cntl_reg.h>  // Real-Time Clock library
#include <driver/rtc_io.h>  // Real-Time Clock Input/Output library
#include <ESPAsyncWebSrv.h>  // Asynchronous web server library
#include <StringArray.h>  // String array library
#include <SPIFFS.h>  // SPI flash file system library

#include "camera_utils.h"  // Local import, placed after standard libraries

// Replace with your network credentials
const char* ssid = "BT-G6CTX8";
const char* password = "qx7qyfYKJHYMg9";

// Azure Custom Vision certificate for HTTPS requests
const char* cert = "-----BEGIN CERTIFICATE-----\n" \
  "MIIIaTCCBlGgAwIBAgITMwCKNV6B37bSEIK7dAAAAIo1XjANBgkqhkiG9w0BAQwF\n" \
  "ADBZMQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u\n" \
  "MSowKAYDVQQDEyFNaWNyb3NvZnQgQXp1cmUgVExTIElzc3VpbmcgQ0EgMDYwHhcN\n" \
  "MjMwMjE3MDQ0NTEwWhcNMjQwMjEyMDQ0NTEwWjBoMQswCQYDVQQGEwJVUzELMAkG\n" \
  "A1UECBMCV0ExEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBD\n" \
  "b3Jwb3JhdGlvbjEaMBgGA1UEAwwRKi5jdXN0b212aXNpb24uYWkwggEiMA0GCSqG\n" \
  "SIb3DQEBAQUAA4IBDwAwggEKAoIBAQDxn26QauuclnDjp49VoBpvWMW3m2Me6dLv\n" \
  "5Yh+nh50KAA3g8N5O/9UcLKmU7rdx9oL0HbUMTaLSa3vKVKtJ1I/qw+dymiv45sA\n" \
  "f6QPZzWIah77WHHaRS7E92ANDFn5PY0yEkBKNJ4uea+NAO1E5+R3ESMFUahQIdFQ\n" \
  "E18+bh7vE2O2IsAxiGLnCbrfKcBFFTvmdEGfnCn6Gkr7XSIQOk+F4NBydCXcxY/P\n" \
  "zb7A7moydOHv7M1v+KwLZ5F/cE9YdYEVZ6F5kb498xI6G7p+x0tukKgPhyEUd676\n" \
  "QYvh12SD8waE8aH91TDUGMfDOzs9b3fp8g6SqpQ1fW0IL/pBUUoZAgMBAAGjggQZ\n" \
  "MIIEFTCCAX8GCisGAQQB1nkCBAIEggFvBIIBawFpAHYAdv+IPwq2+5VRwmHM9Ye6\n" \
  "NLSkzbsp3GhCCp/mZ0xaOnQAAAGGXbnz3AAABAMARzBFAiBsjvB12digfXBm/n29\n" \
  "ylFLvTD9eKb5dNpAbZj4FPwyWwIhAMHkVXP+C/F+owKoJ5LLViQ1ss6wsJvP7x72\n" \
  "HzXR1pmaAHcAc9meiRtMlnigIH1HneayxhzQUV5xGSqMa4AQesF3crUAAAGGXbn0\n" \
  "OgAABAMASDBGAiEA4wHKBcI7hZO1Ll2WtAi5L0ax9kmCzTinen5rJSyd9KgCIQDs\n" \
  "givUj+jqExv+zUc51nNA83RTPWPyqOm9F8VgBnyXkQB2ANq2v2s/tbYin5vCu1xr\n" \
  "6HCRcWy7UYSFNL2kPTBI1/urAAABhl259C8AAAQDAEcwRQIgHn2K0OZLxHqr6MHt\n" \
  "cLDTfMK0eldil2lqPhcWgXEa15oCIQCJFakt9u8/PeNZ0TCsN9ASQhAIcM9IbRm0\n" \
  "Gfq8XhnNjzAnBgkrBgEEAYI3FQoEGjAYMAoGCCsGAQUFBwMCMAoGCCsGAQUFBwMB\n" \
  "MDwGCSsGAQQBgjcVBwQvMC0GJSsGAQQBgjcVCIe91xuB5+tGgoGdLo7QDIfw2h1d\n" \
  "goTlaYLzpz4CAWQCASUwga4GCCsGAQUFBwEBBIGhMIGeMG0GCCsGAQUFBzAChmFo\n" \
  "dHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NlcnRzL01pY3Jvc29mdCUy\n" \
  "MEF6dXJlJTIwVExTJTIwSXNzdWluZyUyMENBJTIwMDYlMjAtJTIweHNpZ24uY3J0\n" \
  "MC0GCCsGAQUFBzABhiFodHRwOi8vb25lb2NzcC5taWNyb3NvZnQuY29tL29jc3Aw\n" \
  "HQYDVR0OBBYEFPAwXc/MoVWfJj4PAGEK6FIw8fHVMA4GA1UdDwEB/wQEAwIEsDAt\n" \
  "BgNVHREEJjAkghEqLmN1c3RvbXZpc2lvbi5haYIPY3VzdG9tdmlzaW9uLmFpMAwG\n" \
  "A1UdEwEB/wQCMAAwZAYDVR0fBF0wWzBZoFegVYZTaHR0cDovL3d3dy5taWNyb3Nv\n" \
  "ZnQuY29tL3BraW9wcy9jcmwvTWljcm9zb2Z0JTIwQXp1cmUlMjBUTFMlMjBJc3N1\n" \
  "aW5nJTIwQ0ElMjAwNi5jcmwwZgYDVR0gBF8wXTBRBgwrBgEEAYI3TIN9AQEwQTA/\n" \
  "BggrBgEFBQcCARYzaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9Eb2Nz\n" \
  "L1JlcG9zaXRvcnkuaHRtMAgGBmeBDAECAjAfBgNVHSMEGDAWgBTVwWc6wqOd9HdS\n" \
  "W1kSOCnmVWi7pTAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwDQYJKoZI\n" \
  "hvcNAQEMBQADggIBAJNTIgHuJ1RrrUPPKwjZmc1Kk9QkClfnwYGvICqyEaYCbae7\n" \
  "rCnOspT6ugkvBUHRj2Db/stutmi/Ask/SNCod+JMMpJ9KcQuH1euq/BqNCSwGtj8\n" \
  "xqT/0eHiANHLqKir9Yv5E+LOhD7+IrwnpZHOf1d1dlbrXnX4UMmoGwxUt0nC4Gv1\n" \
  "1I+DOcJ+D2y/ifEPlCQEghZTs9C922mllZ75hryNoSQfB80GpC8YG44jHczmKEA1\n" \
  "gvcDOk+hcsrIvJrQTKUkHrE2L2Htg/6ayobVmTisilMbwGtxOwYLUh398KecAQzy\n" \
  "/3zTPIthDSZJsVMy8X/UsEsZ+X/dARFvdUrcBcY9n6s/4Cjz0PvFVniGGH0SGUH/\n" \
  "aaExtoW2P/Vs4qZfCYT1285nVtel/j3BUhHgDusM42WjoJ9scUxx3y2eKVZ5qtZy\n" \
  "Hv0GqgIIOu8/Z3xm90avU674yaPvaAlnhUtU7QPsuFrWBAiL3bJBd+jiiQdFkbRd\n" \
  "6EbUVnljMDsNeGRnzcbT7+vlEnmg5r9p/70IfT26ggX65i5svIki86aqbS7DlgQc\n" \
  "wvCkwhgDxNMMJt6dBjKzW+y78UE74IivZjobQaCGOeQHxUHFJ8fL1vdiJoO8PWkS\n" \
  "aM3Cko/uEMp1fhiIUNQdbEaVJ0G65pLY2lFTZ0ar229448BvkD4TRfTK/8vp\n" \
  "-----END CERTIFICATE-----\n";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
boolean take_photo = false;
#define FILE_PHOTO "/photo.jpg"

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // Start SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/funct.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/funct.js", "text/js");
  });

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    take_photo = true;
    request->send_P(200, "text/plain", "Taking Photo");
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

  // Start server
  server.begin();
}

void loop() {
  if (take_photo) {
    cap_and_save();
    take_photo = false;
  }
  delay(1);
}
