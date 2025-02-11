#include <Keypad.h>                // Keypad library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>      // LCD I2C library
#include <WiFi.h>                   // Wi-Fi library

#define RELAY_PIN  19              // Relay pin
#define BUZZER_PIN 23              // Buzzer pin

// Keypad configuration
const char keys[4][3] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[4] = {13, 12, 14, 27}; // Rows pins
byte pin_cols[3] = {26, 25, 33};    // Columns pins

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, 4, 3);

// Password variables
String correctPassword = "123456";
String enteredPassword = "";

// Tone frequency map for the keys
const int toneMap[] = {
  1000,  1050,  1100,  1150,  1200,  1250,  1300,  1350,  1400,  1450,  1500
};

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Wi-Fi configuration
const char* ssid = "Donut";
const char* password = "dddddddd";
WiFiServer server(80);  // Web server on port 80
String header;  // Store HTTP request header

// Variables for web control of the relay
String statePin19 = "off";

// Setup for web control
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);  // Turn off relay initially

  
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();

  lcd.begin();
  lcd.backlight();  // Turn on LCD backlight
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");  // Display message on LCD

}

void loop() {
  // Keypad handling for password entry
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);

    int keyIndex = getKeyIndex(key);
    if (keyIndex != -1) {
      tone(BUZZER_PIN, toneMap[keyIndex], 200);  // Play tone for the key pressed
    }

    if (key == '#') {  // Check password when '#' is pressed
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Checking");
      lcd.setCursor(3, 1);
      lcd.print("Password");
      delay(500);
      lcd.clear();

      if (enteredPassword == correctPassword) {
        lcd.setCursor(4, 0);
        lcd.print("Password");
        lcd.setCursor(4, 1);
        lcd.print("Correct!");
        digitalWrite(RELAY_PIN, HIGH);  // Turn on relay
        tone(BUZZER_PIN, 1000, 300);
        tone(BUZZER_PIN, 1750, 300);
        delay(10000);  // Keep relay on for 10 seconds
        digitalWrite(RELAY_PIN, LOW);  // Turn off relay
      } else {
        lcd.setCursor(4, 0);
        lcd.print("Password");
        lcd.setCursor(3, 1);
        lcd.print("Incorrect!");
        tone(BUZZER_PIN, 500, 300);
        tone(BUZZER_PIN, 250, 300);
      }

      enteredPassword = "";  // Clear entered password
      delay(2000);  // Wait before showing prompt again
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    } else if (key != '*') {
      enteredPassword += key;
      lcd.setCursor(0, 1);
      String passwordDisplay = "";
      for (int i = 0; i < enteredPassword.length(); i++) {
        if (i == enteredPassword.length() - 1) {
          passwordDisplay += enteredPassword[i];
        } else {
          passwordDisplay += "*";
        }
      }
      lcd.print(passwordDisplay);
    }
  }

  // Web server logic to control relay through browser
  WiFiClient client = server.available();
  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /19/on") >= 0) {
              statePin19 = "on";
              digitalWrite(RELAY_PIN, HIGH);  // Turn on relay
                          lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Unlocked");
            delay(1000);  // Show "Locked" for 1 second
            lcd.clear();
            delay(200);
            lcd.setCursor(0, 0);
            lcd.print("Enter Password:");
            } else if (header.indexOf("GET /19/off") >= 0) {
              statePin19 = "off";
              digitalWrite(RELAY_PIN, LOW);   // Turn off relay
              lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Locked");
            delay(1000);  // Show "Unlocked" for 1 second
            lcd.clear();
            delay(200);
            lcd.setCursor(0, 0);
            lcd.print("Enter Password:");
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: monospace; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: red; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 32px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: yellowgreen;}</style></head>");
            client.println("<body><h1>ESP32 Web Server</h1><p>Control Relay State</p>");

            if (statePin19 == "off") {
              client.println("<p><a href=\"/19/on\"><button class=\"button\">CLOSE</button></a></p>");
            } else {
              client.println("<p><a href=\"/19/off\"><button class=\"button button2\">OPEN</button></a></p>");
            }

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
  }
}

// Function to get index of pressed key
int getKeyIndex(char key) {
  switch (key) {
    case '1': return 0;
    case '2': return 1;
    case '3': return 2;
    case '4': return 3;
    case '5': return 4;
    case '6': return 5;
    case '7': return 6;
    case '8': return 7;
    case '9': return 8;
    case '*': return 9;
    case '0': return 10;
    default: return -1;
  }
}