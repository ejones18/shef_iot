/* `traffic_light.ino` script
 *  
 *  Creates a makeshift traffic light system using 3 LEDS on an ESP32 microcontroller.
 *  
 *  Author: Ethan Jones
 *  Date: 2023-02-22
 *  
 *  Notes:
 *  
 *  Due to my ESP32 having some loose connections, the pin number have had to be changed from the ones
 *  suggested in the coursenotes.
 */

int current_colour = 1;
const int red = 10; // LEDs are assigned to pins 6, 9 and 12
const int yellow = 11;
const int green = 12;
const int button = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT); // set up pin 6 as a digital output
  pinMode(button, INPUT_PULLUP); // pin 5: digital input, built-in pullup resistor
  digitalWrite(red, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(button) == LOW){
    Serial.println("\nButton pressed...");
    traffic_light(current_colour);
    delay(1250);
    if (current_colour < 3) {
      current_colour = current_colour+1;
    } else {
      current_colour = 0;
    }
  }
}

void traffic_light(int current_colour){
  switch(current_colour){
    case 0:
      Serial.println("\Traffic lighting for state");
      Serial.print(current_colour);
      digitalWrite(red, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(green, LOW);
      break;
    case 1:
      Serial.println("\Traffic lighting for state");
      Serial.print(current_colour);
      digitalWrite(red, HIGH);
      digitalWrite(yellow, HIGH);
      digitalWrite(green, LOW);
      break;
    case 2:
      Serial.println("\Traffic lighting for state");
      Serial.print(current_colour);
      digitalWrite(red, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(green, HIGH);
      break;
    case 3:
      Serial.println("\Traffic lighting for state");
      Serial.print(current_colour);
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
      delay(100);
      digitalWrite(yellow, LOW);
      delay(100);
      digitalWrite(yellow, HIGH);
      delay(100);
      digitalWrite(yellow, LOW);
      delay(100);
      digitalWrite(yellow, LOW);
      delay(100);
      digitalWrite(yellow, HIGH);
      break;
  }
}
