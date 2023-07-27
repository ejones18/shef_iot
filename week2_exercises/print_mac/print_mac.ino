/* `print_mac.ino` script
 *  
 *  Prints the MAC address of an ESP32 microcontroller.
 *  
 *  Author: Ethan Jones
 *  Date: 2023-02-16
 *  
 */

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  getMAC(MAC_ADDRESS);
  Serial.println("\nsetup...");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("ESP32 MAC = %s\n", MAC_ADDRESS); // print the ESP's "ID"
  delay(1000);

}
