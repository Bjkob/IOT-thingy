#include "DHT.h"
DHT dht;
int clap = 0;
int relay = 8;
bool status_lights = false;
void setup() {
  pinMode(A0, INPUT);
  pinMode(relay, OUTPUT);
  Serial.begin(9600);
  Serial.println("Start");
  Serial.println();
  Serial.println("Status\tHumidity(%)\tTemperature(C) \t(F) \tClap");

  dht.setup(2); // data pin 2
}

void loop() {
  int x = analogRead(A0);
  Serial.println(x);
  /*
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity(); // ดึงค่าความชื้น
  float temperature = dht.getTemperature(); // ดึงค่าอุณหภูมิ
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  */
  Serial.println(clap);
    if(clap <= 1)
    {
      if(x < 1)
      {
        clap++;
        Serial.print("clap = ");
        Serial.println(clap);
      }
    }
    else if(clap == 2)
    {
      if (!status_lights)
        {
          status_lights = true;
          digitalWrite(relay, HIGH);
          Serial.println("Relay HIGH");
          delay(1000);
        }
        else if (status_lights)
        {
          status_lights = false;
          digitalWrite(relay, LOW);
          Serial.println("Relay LOW");
          delay(1000);
        }
        clap = 0;
        Serial.println("Reset clap");
        delay(500);
    }
    else if(temperature > 30)
    (
      digitalWrite(relay, LOW);
    )
  delay(200);
}
