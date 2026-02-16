/*************** BLYNK CONFIG ****************/
#define BLYNK_TEMPLATE_ID "BLYNK ID"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*************** WIFI ****************/
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "WIFI NAME";
char pass[] = "WIFI PASSWORD";

/*************** LCD ****************/
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*************** SENSORS ****************/
#define DHTPIN 4
#define DHTTYPE DHT11
#define GAS_PIN 34
#define GAS_THRESHOLD 1500

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

/*************** MAIN UPDATE FUNCTION ****************/
void updateSystem()
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int gasValue = analogRead(GAS_PIN);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ DHT Sensor Error");
    return;
  }

  /******** SERIAL LOGGING ********/
  Serial.println("========== SENSOR DATA ==========");
  Serial.print("Temperature (C): ");
  Serial.println(temperature);

  Serial.print("Humidity (%): ");
  Serial.println(humidity);

  Serial.print("Gas Raw Value (0-4095): ");
  Serial.println(gasValue);

  Serial.print("WiFi Signal Strength (RSSI): ");
  Serial.println(WiFi.RSSI());

  Serial.print("Blynk Status: ");
  if (Blynk.connected())
    Serial.println("CONNECTED");
  else
    Serial.println("DISCONNECTED");

  Serial.println("=================================\n");

  /******** SEND DATA TO BLYNK ********/
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, gasValue);

  if (gasValue > GAS_THRESHOLD) {
    Blynk.logEvent("pollution_alert", "⚠️ BAD AIR DETECTED!");
  }

  /******** LCD DISPLAY ********/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C H:");
  lcd.print(humidity);

  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(gasValue);

  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  if (gasValue < GAS_THRESHOLD)
    lcd.print("Air: Fresh");
  else
    lcd.print("Air: BAD !");
}

/*************** SETUP ****************/
void setup()
{
  Serial.begin(115200);
  delay(1000);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");
  lcd.setCursor(0,1);
  lcd.print("Please wait...");

  Serial.println("Starting System...");
  Serial.println("Connecting to WiFi...");

  Blynk.begin(auth, ssid, pass);

  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();

  timer.setInterval(5000L, updateSystem);
}

/*************** LOOP ****************/
void loop()
{
  Blynk.run();
  timer.run();
}