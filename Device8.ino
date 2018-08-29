//device8 센서값(온도습도밝기거리) + thingspeak + 1초측정,20초마다전송 + 배열
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

//디스플레이부분
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN D3
#define DHTTYPE DHT22

//초음파센서부분
#define TrigPin  D7
#define EchoPin  D8

//디스플레이부분
#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//DHT 센서 초기화
DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi Settings
const char* ssid = "Pocket"; // your wireless network name (SSID)
const char* password = "20160606"; // your Wi-Fi network password
//const char* ssid = "free_wifi"; // your wireless network name (SSID)
//const char* password = "iloveyou"; // your Wi-Fi network password
WiFiClient client;

// ThingSpeak Settings
unsigned long No1 = 556663;
const char * Key1 = "FK33H4Y1BWBL3B1W";
unsigned long No2 = 556664;
const char * Key2 = "G0LNYM30E6SHI8KW";
unsigned long No3 = 556665;
const char * Key3 = "G874RVZNJ09CKCPJ";
unsigned long No4 = 556667;
const char * Key4 = "WD7GP0YDTTXFVWPJ";

//ADC_MODE(ADC_VCC);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  delay(10);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  //display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi..");
  display.display();

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  ThingSpeak.begin(client);

  //초음파센서부분
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  Serial.println("Start!");
  display.println("Start!");
  display.display();

  delay(1000);
}

void loop()
{
  float temp[20];
  float temptotal = 0;
  float tempave = 0;
  float hum[20];
  float humtotal = 0;
  float humave = 0;
  long rssi = 0;
  int lux[20];
  int luxtotal = 0;
  int luxave = 0;
  //  float voltage[20];
  long duration;
  float distance[20];
  float distancemin = 150;
  //int i=0

  for (int i = 0; i < 20; i++) {
    temp[i] = dht.readTemperature();
    hum[i] = dht.readHumidity();
    lux[i] = analogRead(A0);
    //voltage[i] = ESP.getVcc() / 1000.0;
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);
    duration = pulseIn(EchoPin, HIGH);
    distance[i] = duration * 0.034 / 2;
    rssi = WiFi.RSSI();

    temptotal = temp[i] + temptotal;
    humtotal = hum[i] + humtotal;
    luxtotal = lux[i] + luxtotal;
    if (distance[i] < distancemin) {
      distancemin = distance[i];
    }

    Serial.print("Temperature :");
    Serial.print(temp[i]);
    Serial.print("C,  ");
    Serial.print("Humidity :");
    Serial.print(hum[i]);
    Serial.print("%,  ");
    Serial.print("Illuminance :");
    Serial.print(lux[i]);
    Serial.print("LUX,  ");
    Serial.print("Distance :");
    Serial.print(distance[i]);
    Serial.print("cm,  ");
    //  Serial.print("Voltage :");
    //  Serial.print(voltage[i]);
    //  Serial.print("V,  ");
    Serial.print("RSSI :");
    Serial.println(rssi);

    display.clearDisplay();
    //display.display();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("TEMP : ");
    display.println(temp[i]);
    display.print("HUMI : ");
    display.println(hum[i]);
    display.print("DIST : ");
    display.println(distance[i]);
    display.print("ILLU : ");
    display.println(lux[i]);
    //  display.print("VOLT : ");
    //  display.println(voltage[i]);
    display.print("SIGN : ");
    display.println(rssi);
    display.print((i + 1) * 5);
    display.println("%");
    display.display();
    delay(1000);
  }

  tempave = temptotal / 20;
  humave = humtotal / 20;
  luxave = luxtotal / 20;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Sending..");
  display.print("TEMP : ");
  display.println(tempave);
  display.display();
  ThingSpeak.writeField(No1, 1, tempave, Key1);
  display.print("HUMI : ");
  display.println(humave);
  display.display();
  ThingSpeak.writeField(No2, 1, humave, Key2);
  display.print("DIST : ");
  display.println(distancemin);
  display.display();
  ThingSpeak.writeField(No3, 1, luxave, Key3);
  display.print("ILLU : ");
  display.println(luxave);
  //  display.print("VOLT : ");
  //  display.println(voltage[i]);
  display.display();
  ThingSpeak.writeField(No4, 1, distancemin, Key4);
  display.print("OK");
  //  display.print("VOLT : ");
  //  display.println(voltage[i]);
  display.display();
  delay(100);
  // ThingSpeak will only accept updates every 15 seconds.
}
