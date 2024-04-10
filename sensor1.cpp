#include <ESP8266WiFi.h>
#include "DHT.h"
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
 
#define ssid "Cablex Management"
#define password "Management.1989"
#define DPIN 2        // Pin to connect DHT sensor (GPIO number) D4 on ESP-12F
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type
 
DHT dht(DPIN, DTYPE);
 
// WiFi AP SSID
#define WIFI_SSID "Cablex Management"
// WiFi password
#define WIFI_PASSWORD "Management.1989"
#define INFLUXDB_URL "http://172.16.29.64:8086"
#define INFLUXDB_TOKEN "OQlxqzeXoLsl0L91U1BRhYDIbP-dvkc-dU6sCfLkAA_hnbA7TdeLNGjNBe0hgznprPFcFTwNN6JF64CSpLnSZg=="
#define INFLUXDB_ORG "fb25ccc3a8ce5816"
#define INFLUXDB_BUCKET "Test"
// Time zone info
#define TZ_INFO "UTC1"
 
// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
// Declare Data point
Point sensor("Lab_sensor_4");
 
void setup() {
  Serial.begin(9600);
  dht.begin();
 
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi network...");
  }
 
  // Show the connected network
  Serial.print("Network: ");
  Serial.println(WiFi.SSID());
 
  // Show IP address
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
 
  // Add tags to the data point
  sensor.addTag("device", "ESP8266");
  sensor.addTag("SSID", WiFi.SSID());
}
 
void loop() {
 
  float tc = dht.readTemperature(false);  // Read temperature in Celsius
  float tf = dht.readTemperature(true);   // Read temperature in Fahrenheit
  float hu = dht.readHumidity();          // Read humidity
 
  Serial.print("Temp: ");
  Serial.print(tc);
  Serial.print(" C, ");
  Serial.print(tf);
  Serial.print(" F, Hum: ");
  Serial.print(hu);
  Serial.println("%");
 
  // Clear fields for reusing the point. Tags will remain the same as set above.
  sensor.clearFields();
 
  // Store measured value into point
  sensor.addField("temperature_C", tc);
  sensor.addField("temperature_F", tf);
  sensor.addField("humidity", hu);
  sensor.addField("rssi", WiFi.RSSI());
 
  // Print what we are exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());
 
  // Check WiFi connection and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
    WiFi.reconnect();
  }
 
  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
 
  Serial.println("Waiting 1 second");
  delay(60000);
 
  Serial.println("Configured by Yadel and Hugo");
 
 
}
 
 