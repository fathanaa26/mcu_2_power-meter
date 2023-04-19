
#include <Servo.h>
#include <Stepper.h>
//#include <PZEM004Tv30.h>
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
#define INFLUXDB_TOKEN "I_E9baHRmDDWhPqK6aHWQky_GDZcXrc0CeDwjvxZjVTG1Rw5eQ8XSpbMA70WO2VHCcPzcqiigOqWLQ-7PjJ6Aw=="
#define INFLUXDB_ORG "2156806bb2bcc045"
#define INFLUXDB_BUCKET "pama-hev"

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

PZEM004Tv30 pzem_r(&Serial2);
Servo myServo;
Stepper myStepper(stepsPerRevolution, in1, in3, in2, in4);

void setup() {

  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
  Serial.begin(115200);
  myStepper.setSpeed(5);
  // Serial2.begin(9600, SERIAL_8N1, rxd2, txd2);
  myServo.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potensioPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  stepperTest();

  vac_sensor.clearField();
  iac_sensor.clearField();
  freq_sensor.clearField();
  energyAC_sensor.clearField();
  powerAC_sensor.clearField();
  pf_sensor.clearField();

  vac_sensor.addField();
  iac_sensor.addField();
  freq_sensor.addField();
  energyAC_sensor.addField();
  powerAC_sensor.addField();
  pf_sensor.addField();
  
  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(rpm_sensor));
  
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }
  
  if (!client.writePoint(rpm_sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

}
