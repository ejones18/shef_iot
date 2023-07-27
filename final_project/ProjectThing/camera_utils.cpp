/**
 * @file camera_utils.cpp
 * @brief Contains utilities for capturing photos using an ESP-EYE camera and saving them to SPIFFS.
 *
 * @author Ethan Jones
 * @date 2023-04-17
 */

#include "Arduino.h"
#include <FS.h>
#include "esp_camera.h"
#include "SPIFFS.h"

#define FILE_PHOTO "/photo.jpg"

/**
 * @brief Checks if the last photo capture was successful.
 * @param fs A reference to the file system object to use for reading the file.
 * @return True if the file size is greater than 100 bytes, indicating that the photo was captured successfully. False otherwise.
 */
bool check_img( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

/**
 * @brief Captures a photo using the ESP32 camera and saves it to SPIFFS.
 * @details This function takes a photo using the camera and saves it to the file specified by the FILE_PHOTO macro. If the photo capture is unsuccessful, the function will retry until a successful capture is made.
 */
void cap_and_save() {
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in SPIFFS
    ok = check_img(SPIFFS);
  } while ( !ok );
}