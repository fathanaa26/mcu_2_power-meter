
// #include <Servo.h>
#include <Stepper.h>
#include <PZEM004Tv30.h>
#define rxd2 16
#define txd2 17

#define servoPin 19
#define in1 18
#define in2 27
#define in3 4
#define in4 5
#define buzzer 13
#define potensioPin 34
#define buttonPin 14
#define led 19

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>

#define WIFI_SSID "PDP Atas"
#define WIFI_PASSWORD "kumahaaa"
#define INFLUXDB_URL "http://10.133.226.235:8086/"
#define INFLUXDB_TOKEN "_B3Hdbv93tOSKqqMzIMuxY91J8TeQVoL3JbP5_EnQUSb-Ig_8Q9qQRvd46W7YtUBYQzVSYP8IGoV0wcI7rvxNQ=="
#define INFLUXDB_ORG "c7480b219644ffa4"
#define INFLUXDB_BUCKET "power-meter"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

Point vac_sensor("vac_sensor");
Point iac_sensor("iac_sensor");
Point freq_sensor("rfreq_sensor");
Point energyAC_sensor("energyAC_sensor");
Point powerAC_sensor("powerAC_sensor");
Point pf_sensor("pf_sensor");

const int stepsPerRevolution = 2048;
float vac;
float iac;
float freq;
float energyAC;
float powerAC;
float pf;
int pos;
bool modeState = 0;
int buttonValue;
int rawValue;
int percentage;

PZEM004Tv30 pzem_r();
// Servo myServo;
Stepper myStepper(stepsPerRevolution, in1, in3, in2, in4);

static uint32_t previousMillis;

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "GMT-7"

void setup() {
  Serial.begin(115200);	

  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  client.setWriteOptions(WriteOptions().writePrecision(WritePrecision::MS));
  client.setWriteOptions(WriteOptions().batchSize(6));
  client.setWriteOptions(WriteOptions().bufferSize(24));
  client.setWriteOptions(WriteOptions().flushInterval(10));

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  myStepper.setSpeed(5);
  Serial2.begin(9600, SERIAL_8N1, rxd2, txd2);
  // myServo.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potensioPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  
  // stepperTest();
  readPower();
  // readMode();
  // readPot();
  // servoTest();
  // buzzerTest();

  vac_sensor.clearFields();
  iac_sensor.clearFields();
  freq_sensor.clearFields();
  energyAC_sensor.clearFields();
  powerAC_sensor.clearFields();
  pf_sensor.clearFields();

  vac_sensor.addField("vac_value", vac);
  iac_sensor.addField("iac_value", iac);
  freq_sensor.addField("freq_value", freq);
  energyAC_sensor.addField("energyAC_value", energyAC);
  powerAC_sensor.addField("powerAC_value", powerAC);
  pf_sensor.addField("pf_value", pf);

  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

  if(millis() - previousMillis >= 20){

    Serial.println("# Start Writing #");
    Serial.println(client.pointToLineProtocol(vac_sensor));
    Serial.println(client.pointToLineProtocol(iac_sensor));
    Serial.println(client.pointToLineProtocol(freq_sensor));
    Serial.println(client.pointToLineProtocol(energyAC_sensor));
    Serial.println(client.pointToLineProtocol(powerAC_sensor));
    Serial.println(client.pointToLineProtocol(pf_sensor));
    Serial.println("# Writing End #");

    client.writePoint(vac_sensor);
    client.writePoint(iac_sensor);
    client.writePoint(freq_sensor);
    client.writePoint(energyAC_sensor);
    client.writePoint(powerAC_sensor);
    client.writePoint(pf_sensor);

    if (!client.isBufferEmpty()) {
      client.flushBuffer();
    }

    previousMillis += 20;
  }
  
  

}
