#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- WIFI ----------
const char* ssid = "ESP32_ROBOT";
const char* password = "Your_password";
WiFiServer server(80);

// ---------- ULTRASONIC ----------
#define TRIG_PIN 5
#define ECHO_PIN 34

// ---------- MOTOR ----------
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12
#define ENA 25
#define ENB 33

// ---------- SD ----------
#define SD_CS 4

float distance;
unsigned long lastAction = 0;
String mode = "AUTO";

// ---------- MOTOR ----------
void moveForward(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
}

void reverseMove(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
}

void stopMotors(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
}

// ---------- PIVOT ----------
void pivotRight(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
}

void pivotLeft(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
}

// ---------- SENSOR ----------
float readDistance(){
  digitalWrite(TRIG_PIN,LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);
  long d = pulseIn(ECHO_PIN,HIGH,30000);
  return d*0.034/2;
}

// ---------- LOG ----------
void logData(float d){
  File f = SD.open("/log.txt", FILE_APPEND);
  if(f){
    f.print(millis());
    f.print(" ms | ");
    f.print(d);
    f.println(" cm");
    f.close();
  }
}

// ---------- OLED ----------
void updateOLED(String status){
  display.clearDisplay();

  display.setCursor(0,0);
  display.print("Mode: ");
  display.println(mode);

  display.setCursor(0,12);
  display.print("Dist: ");
  display.print(distance);
  display.println(" cm");

  display.setCursor(0,24);
  display.print("Status:");

  display.setCursor(0,36);
  display.println(status);

  display.display();
}

// ---------- ANIMATION ----------
void showChecking(String msg){
  for(int i=0;i<3;i++){
    display.clearDisplay();

    display.setCursor(0,0);
    display.print("Mode: ");
    display.println(mode);

    display.setCursor(0,12);
    display.print("Dist: ");
    display.print(distance);
    display.println(" cm");

    display.setCursor(0,24);
    display.print(msg);

    display.setCursor(0,36);
    for(int j=0;j<=i;j++) display.print(".");

    display.display();
    delay(150);
  }
}

// ---------- ADAPTIVE CONTROL ----------
void adaptiveControl(){

  int speed;
  int turnTime;

  if(distance <= 10){
    speed = 0;
    turnTime = 800;
  }
  else if(distance <= 20){
    speed = 90;
    turnTime = 700;
  }
  else if(distance <= 30){
    speed = 120;
    turnTime = 500;
  }
  else{
    speed = 150;
    turnTime = 0;
  }

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);

  if(distance <= 10){
    stopMotors();
    updateOLED("STOP");
    logData(distance);
  }

  else if(distance <= 30){

    updateOLED("WARNING");

    if(millis() - lastAction > 1500){

      updateOLED("REVERSING");
      reverseMove();
      delay(500);

      stopMotors();
      delay(200);

      showChecking("CHECK RIGHT");
      pivotRight();
      delay(turnTime);

      stopMotors();
      delay(200);

      float newDist = readDistance();

      if(newDist <= 30 && newDist > 0){
        showChecking("CHECK LEFT");
        pivotLeft();
        delay(turnTime + 100);
      }

      stopMotors();
      logData(distance);
      lastAction = millis();
    }
  }

  else{
    moveForward();
    updateOLED("SAFE");
  }
}

// ---------- HTML ----------
String htmlPage(){
  return R"rawliteral(
  <html>
  <body style='text-align:center; font-family:Arial;'>
  <h2>Smart Robot</h2>

  <canvas id="joy" width="300" height="300"
  ontouchmove="move(event)" ontouchend="stop()"></canvas>

  <script>
  let c=document.getElementById("joy");
  let ctx=c.getContext("2d");

  function draw(x,y){
    ctx.clearRect(0,0,300,300);
    ctx.beginPath(); ctx.arc(150,150,100,0,2*Math.PI); ctx.stroke();
    ctx.beginPath(); ctx.arc(x,y,30,0,2*Math.PI); ctx.fill();
  }

  function move(e){
    let r=c.getBoundingClientRect();
    let x=e.touches[0].clientX-r.left;
    let y=e.touches[0].clientY-r.top;
    draw(x,y);
    fetch(`/joy?x=${x-150}&y=${150-y}`);
  }

  function stop(){
    draw(150,150);
    fetch(`/joy?x=0&y=0`);
  }
  </script>

  <br><br>
  <button onclick="fetch('/AUTO')">AUTO</button>
  <button onclick="fetch('/MANUAL')">MANUAL</button>

  </body></html>
  )rawliteral";
}

// ---------- SETUP ----------
void setup(){
  Serial.begin(115200);

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);

  pinMode(IN1,OUTPUT); pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT); pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT); pinMode(ENB,OUTPUT);

  SD.begin(SD_CS);

  Wire.begin(21,22);
  display.begin(0x3C,true);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  WiFi.softAP(ssid,password);
  server.begin();
}

// ---------- LOOP ----------
void loop(){

  distance = readDistance();

  WiFiClient client = server.available();

  if(client){
    String req = client.readStringUntil('\r');

    if(req.indexOf("/AUTO")!=-1) mode="AUTO";
    if(req.indexOf("/MANUAL")!=-1) mode="MANUAL";

    if(req.indexOf("/joy")!=-1 && mode=="MANUAL"){
      int x=req.substring(req.indexOf("x=")+2,req.indexOf("&")).toInt();
      int y=req.substring(req.indexOf("y=")+2).toInt();

      if(distance > 30){
        if(y > 30) moveForward();
        else if(y < -30) reverseMove();
        else if(x > 30) pivotRight();
        else if(x < -30) pivotLeft();
        else stopMotors();
      }
    }

    client.println("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
    client.print(htmlPage());
    client.stop();
  }

  if(mode=="AUTO"){
    adaptiveControl();
  }
  else{
    updateOLED("MANUAL");
  }

  delay(40);
}
