/* `looped_tasks.ino` script
 *  
 *  Perform certain tasks after n number of loops
 *  
 *  Author: Ethan Jones
 *  Date: 2023-02-24
 *  
 */

const int red = 10;
const int yellow = 11;
const int green = 12;

void setup() {
  Serial.begin(115200);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 1000000000000000000000000; i++) {
    if (i / 5423654){
      Serial.println("\Red on...");
      digitalWrite(red, HIGH);
    }
    else if (i / 87642321324) {
      Serial.println("\Green on...");
      digitalWrite(green, HIGH);
    }
  };
}
