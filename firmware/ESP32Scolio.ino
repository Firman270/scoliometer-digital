#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <LiquidCrystal_I2C.h>

/* WIFI */
const char* ssid = "xxxxxx";
const char* password = "xxxxxx";
String baseUrl = "ENDPOINT_FROM_WEBSITE";

/* LCD 16x4 */
LiquidCrystal_I2C lcd(0x27,16,4);

/* MPU */
MPU6050 mpu;

/* PIN */
#define BTN_POSITION  25
#define BTN_STARTSTOP 26
#define LED_GREEN     2
#define LED_RED       4
#define BUZZER        5
#define BATTERY_PIN   34

/* SENSOR */
int16_t ax, ay, az, gx, gy, gz;
float pitch, pitchAcc, atrAngle;

/* KALMAN */
float Q_angle=0.001, Q_bias=0.003, R_measure=0.03;
float anglePitch=0,biasPitch=0;
float PPitch[2][2]={{1,0},{0,1}};

/* TIME */
unsigned long prevTime, lcdTimer;
float dt;

/* SYSTEM */
int currentPatientId=0;
bool patientReady=false;
String patientName="---";

/* QUEUE */
unsigned long lastQueryTime = 0;
const unsigned long queryInterval = 5000;

/* POSITION */
String positions[3]={"Thoracic","Lumbar","Cervical"};
int positionIndex=0;

/* ATR */
float zeroOffset=0;
bool measuring=false;
float maxATR=0;

/* BUTTON */
bool lastBtnPos=HIGH;
bool lastBtnStart=HIGH;

/* ======================= */

/* KALMAN */
float kalmanUpdate(float newAngle,float newRate,float &angle,float &bias,float P[2][2]){
  angle+=dt*(newRate-bias);
  P[0][0]+=dt*(dt*P[1][1]-P[0][1]-P[1][0]+Q_angle);
  P[0][1]-=dt*P[1][1];
  P[1][0]-=dt*P[1][1];
  P[1][1]+=Q_bias*dt;

  float S=P[0][0]+R_measure;
  float K0=P[0][0]/S;
  float K1=P[1][0]/S;

  float y=newAngle-angle;

  angle+=K0*y;
  bias+=K1*y;

  float P00=P[0][0];
  float P01=P[0][1];

  P[0][0]-=K0*P00;
  P[0][1]-=K0*P01;
  P[1][0]-=K1*P00;
  P[1][1]-=K1*P01;

  return angle;
}

/* WIFI */
void connectWiFi(){
  WiFi.begin(ssid,password);
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
  }
  lcd.clear();
  lcd.print("WiFi OK");
  delay(1000);
}

/* AUTO ZERO */
void autoZero(){
  float sum=0;
  for(int i=0;i<200;i++){
    mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
    float accY=ay/16384.0;
    float accZ=az/16384.0;
    float angle=atan2(accY,accZ)*180/PI;
    sum+=angle;
    delay(5);
  }
  zeroOffset = sum/200;
}

/* READ */
void readAngle(){
  mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);

  float accY=ay/16384.0;
  float accZ=az/16384.0;
  float gyroY=gx/131.0;

  pitchAcc=atan2(accY,accZ)*180/PI;
  pitch=kalmanUpdate(pitchAcc,gyroY,anglePitch,biasPitch,PPitch);

  atrAngle = pitch - zeroOffset;
}

/* BATTERY */
float getBatteryVoltage(){
  float sum = 0;

  for(int i=0;i<20;i++){
    sum += analogRead(BATTERY_PIN);
    delay(2);
  }

  float raw = sum / 20.0;

  // convert ADC ke volt
  float vADC = raw * (3.3 / 4095.0);

  // 🔥 kalibrasi divider (biasanya 5.0, tapi kita adjust dikit)
  float batteryVoltage = vADC * 5.2;

  return batteryVoltage;
}

int getBatteryPercent(){

  float v = getBatteryVoltage();

  float percent = (v - 3.0) / (4.2 - 3.0) * 100.0;

  if(percent > 100) percent = 100;
  if(percent < 0) percent = 0;

  return (int)percent;
}

/* FETCH QUEUE */
void fetchQueue(){

  if(WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(baseUrl + "/check-queue");
  http.addHeader("Accept","application/json");

  int code = http.GET();

  if(code == 200){

    String payload = http.getString();

    StaticJsonDocument<300> doc;
    if(!deserializeJson(doc,payload)){

      if(doc["status"] == "ready"){

        currentPatientId = doc["patient_id"];
        patientName = doc["patient_name"].as<String>();

        patientReady = true;

        beep();
      }
    }
  }

  http.end();
}

/* SEND */
void sendMeasurement(float angle){

  if(WiFi.status() != WL_CONNECTED || currentPatientId==0){
    digitalWrite(LED_RED,HIGH);
    return;
  }

  HTTPClient http;

  http.begin(baseUrl + "/send-measure");
  http.addHeader("Content-Type","application/json");
  http.addHeader("Accept","application/json");

  StaticJsonDocument<200> doc;
  doc["patient_id"]=currentPatientId;
  doc["atr_angle"]=angle;
  doc["position"]=positions[positionIndex];

  String json;
  serializeJson(doc,json);

  int code = http.POST(json);

  if(code == 200){
    digitalWrite(LED_GREEN,HIGH);
    delay(1000);
    digitalWrite(LED_GREEN,LOW);
  }else{
    digitalWrite(LED_RED,HIGH);
  }

  http.end();
}

/* BUZZER */
void beep(){
  digitalWrite(BUZZER,HIGH);
  delay(100);
  digitalWrite(BUZZER,LOW);
}

/* ======================= */

void setup(){

  Serial.begin(115200);
  analogSetPinAttenuation(BATTERY_PIN, ADC_11db);
  Wire.begin(21,22);

  pinMode(BTN_POSITION,INPUT_PULLUP);
  pinMode(BTN_STARTSTOP,INPUT_PULLUP);

  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(BUZZER,OUTPUT);

  lcd.init();
  lcd.backlight();

  connectWiFi();

  fetchQueue(); // 🔥 langsung ambil data saat nyala

  mpu.initialize();

  autoZero();

  prevTime=millis();
}

/* ======================= */

void loop(){

  dt=(millis()-prevTime)/1000.0;
  prevTime=millis();

  readAngle();

  /* FETCH QUEUE */
  if(!patientReady && millis() - lastQueryTime > queryInterval){
    lastQueryTime = millis();
    fetchQueue();
  }

  /* PEAK */
  if(measuring){
    if(abs(atrAngle)>maxATR){
      maxATR=abs(atrAngle);
    }
  }

  /* LCD 16x4 */
  if(millis()-lcdTimer>300){
    lcdTimer=millis();

    lcd.setCursor(0,0);
    lcd.print("ATR:");
    lcd.print(atrAngle,1);
    lcd.print("    ");

    lcd.setCursor(0,1);
    lcd.print("Pos:");
    lcd.print(positions[positionIndex]);
    lcd.print("   ");

  lcd.setCursor(0,2);
  if(patientReady){
    lcd.print("Name:");
    lcd.print(patientName);
    lcd.print("   ");
  }else{
    lcd.print("Waiting patient ");
  }

    lcd.setCursor(0,3);
    lcd.print("Bat:");
    lcd.print(getBatteryPercent());
    lcd.print("%   ");
  }

  /* BUTTON POSITION */
  if(digitalRead(BTN_POSITION)==LOW && lastBtnPos==HIGH){
    positionIndex = (positionIndex+1)%3;
    beep();
  }
  lastBtnPos=digitalRead(BTN_POSITION);

  /* BUTTON START/STOP */
  if(digitalRead(BTN_STARTSTOP)==LOW && lastBtnStart==HIGH){

    measuring = !measuring;

    if(measuring){

      if(!patientReady){
    measuring=false;
    beep();
    lcd.setCursor(0,3);
    lcd.print("No Patient!     ");
    delay(1000);
    return;
  }

      maxATR=0;
      digitalWrite(LED_GREEN,HIGH);
      beep();

    }else{

      digitalWrite(LED_GREEN,LOW);
      beep(); beep();

      sendMeasurement(maxATR);
    }
  }
  lastBtnStart=digitalRead(BTN_STARTSTOP);
}
