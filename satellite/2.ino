/* --------   libraries   -------- */
#include <Int64String.h> // int ot string
#include <Arduino.h>
#include <Wire.h>
#include "radSens1v2.h" // rad sensor
#include <TroykaMQ.h> // gas sensors
#include <MS5611.h> // presure sensor & termometer
#include <OneWire.h> // termometers ds18b20
#include <Servo.h> // servo
double referencePressure;
MS5611 ms5611;
 
// 0 - монитор порта, 1 - передатчик, 2 - SD-карта, 3 - GPS
 
int temt6000Pin = A7; // gas sensors
const int GY8511 = A8; // gas sensors
 
ClimateGuard_RadSens1v2 radSens(RS_DEFAULT_I2C_ADDRESS); // rad sensor
 
/* --------   gas sensors   -------- */
#define PIN_MQ2 A2 
#define PIN_MQ135 A3 
#define PIN_MQ9 A4 
#define PIN_MQ8 A5 
 
MQ2 mq2(PIN_MQ2); 
MQ135 mq135(PIN_MQ135); 
MQ9 mq9(PIN_MQ9); 
MQ8 mq8(PIN_MQ8);
 
 
/* --------   termometers   -------- */
OneWire  ds(10); 
 
byte adr1[8] = {0x28, 0xFD, 0x6D, 0x41, 0xD, 0x0, 0x0, 0xA4}; 
byte adr2[8] = {0x28, 0x5A, 0x95, 0x3F, 0xD, 0x0, 0x0, 0x58};
byte adr3[8] = {0x28, 0xAA, 0xA, 0x45, 0x51, 0x14, 0x1, 0x8};
 
#define printing_lamp 37 // printing indicator
#define heating_lamp 36 // heating indicator
 
Servo myservo; // servo
 
void start_termo(byte addr[8]) {  // first part of getting temperature
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
}
 
float get_temp(byte addr[8]) { // second part of getting temperature
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
 
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
 
  for (i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
 
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3;
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  
    else if (cfg == 0x20) raw = raw & ~3; 
    else if (cfg == 0x40) raw = raw & ~1; 
  }
  float celsius = (float)raw / 16.0;
  return celsius;
}
 
void frint(String output) { // print data to all Serials
  Serial.print(output);
  Serial1.print(output);
  Serial2.print(output);
}
 
void frintln(String output) { // print data to all Serials + new line
  Serial.println(output);
  Serial1.print(output + "\n");
  Serial2.println(output);
}
 
/* --------   servo   -------- */
int servo_pos = 0;
int servo_dir = 1;
 
void setup() { 
  Serial.begin(57600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600); // скорость передачи данных
 
  frintln("!lattitude,longitude,altitude_GPS,time,radiation_dinamic,radiation_static,LPG,CH4,CO,H2,CO2,LPG(2),CH4(2),CO(2),H2(2),intensivity,brightness,temperature_pres_sens,pressure,altitude_pres_sens,temperature_electronics,temperature_battery,temperature_outside,current,voltage,heating");       
 
  /* --------   rad sensor   -------- */
  radSens.radSens_init();
  uint8_t sensorChipId = radSens.getChipId();
  uint8_t firmWareVer = radSens.getFirmwareVersion();
  uint16_t sensitivity = radSens.getSensitivity();
  radSens.setSensitivity(55);
  sensitivity = radSens.getSensitivity();
  radSens.setSensitivity(105);
  bool hvGeneratorState = radSens.getHVGeneratorState();
  radSens.setHVGeneratorState(true);
  hvGeneratorState = radSens.getHVGeneratorState();
  bool ledState = radSens.getLedState();
  radSens.setLedState(true);
  ledState = radSens.getLedState();
 
  /* --------   gas sensors   -------- */
  pinMode(42, OUTPUT); 
  digitalWrite(42, HIGH); 
  pinMode(43, OUTPUT); 
  digitalWrite(43, HIGH); 
  pinMode(44, OUTPUT); 
  digitalWrite(44, HIGH); 
  pinMode(45, OUTPUT); 
  digitalWrite(45, HIGH); 
  pinMode(46, OUTPUT); 
  digitalWrite(46, LOW); 
  pinMode(47, OUTPUT); 
  digitalWrite(47, LOW); 
 
  mq2.calibrate(); 
  mq135.calibrate(); 
  mq9.calibrate(); 
  mq8.calibrate();
 
  /* --------   heater   -------- */
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
 
  /* --------   termometers ds18b20   -------- */
  //start_termo(adr1);
  //start_termo(adr2);
  //start_termo(adr3);
 
  pinMode(printing_lamp, OUTPUT); // printing indicator 
  pinMode(heating_lamp, OUTPUT); // heating indicator
 
  myservo.attach(5); // servo
 
  /* --------   barometer   -------- */
  ms5611.begin(MS5611_HIGH_RES);
  referencePressure = ms5611.readPressure();
 
  pinMode(temt6000Pin, INPUT); // Brightness sensor
}
 
/* --------   parsing data from GPS   -------- */
String Data[3] {"WD,", "WD,", "WD,"}; 
boolean getStarted = false;
String string_convert = "";
int index = 0;
boolean recievedFlag = false;
void parsing() {
  if (Serial3.available() > 0) {
    char incomingByte = Serial3.read();      
    if (getStarted) {                        
      if (incomingByte != ',' && incomingByte != '\n') {  
        string_convert += incomingByte;      
      } else {
        //Serial.print("->");
        //Serial.print(string_convert);
        //Serial.println(",");                               
        if (index == 0) {
          if (string_convert != "GNGGA") {
            getStarted = false; 
            recievedFlag = false;
          }
        } else if (index == 2) {
          if (string_convert == "") {
            Data[0] = "WD";
          } else {
            Data[0] = string_convert;
          }
        } else if (index == 4) {
          if (string_convert == "") {
            Data[1] = "WD";
          } else {
            Data[1] = string_convert;
          }
        } else if (index == 9) {
          if (string_convert == "") {
            Data[2] = "WD";
          } else {
            Data[2] = string_convert;
          }
        }
        string_convert = "";                  
        index++;                             
      }
    }
    if (incomingByte == '$') {               
      getStarted = true;                    
      index = 0;                            
      string_convert = ""; 
      recievedFlag = false;               
    }
    if (incomingByte == '\n' && getStarted) {             
      getStarted = false; 
      recievedFlag = true;
    }
  }
}
 
unsigned long long time = 0; // time
unsigned long long upd_time = 0; // time of last update from GPS
unsigned long long last_time = 0; // time for servo
byte heat = 0; // heater
 
/* --------   pringting function   -------- */
 
long long last_print_time = 0;
void printData(String &output) {
  /* --------   starting termometers 1 and 3   -------- */
  start_termo(adr1);
  start_termo(adr3);
 
  /* --------   time   -------- */
  //frint("time:");
  time = millis();
  output += int64String(time / 1000);
  output += ",";
 
  /* --------   radiation   -------- */
  long long rd = radSens.getRadIntensyDyanmic() * 100;
  long long rs = radSens.getRadIntensyStatic() * 100;
  //frint("rd:");
  output += int64String(rd / 100) + "." + int64String(rd % 100);
  output += ",";
  //frint("rs:");
  output += int64String(rs / 100) + "." + int64String(rs % 100);
  output += ",";
 
  /* --------   gas sensors   -------- */
  //frint("LPG:"); 
  output += int64String((long long)mq2.readLPG()); 
  output += ","; 
  //frint("CH4:");
  output += int64String((long long)mq2.readMethane()); 
  output += ","; 
  //frint("CO:");
  output += int64String((long long)mq2.readSmoke()); 
  output += ","; 
  //frint("H2:");
  output += int64String((long long)mq2.readHydrogen()); 
  output += ","; 
  //frint("CO2:");
  output += int64String((long long)mq135.readCO2()); 
  output += ","; 
  //frint("LPG2:");
  output += int64String((long long)mq9.readLPG()); 
  output += ","; 
  //frint("CH4(2):");
  output += int64String((long long)mq9.readMethane()); 
  output += ","; 
  //frint("CO(2):");
  output += int64String((long long)mq9.readCarbonMonoxide()); 
  output += ","; 
  //frint("H2(2):");
  output += int64String((long long)mq8.readHydrogen());
  output += ","; 
 
  /* --------   brightness & intensivity   -------- */
  int value = analogRead(GY8511); 
  //frint("Iny:"); 
  output += int64String((long long)value); 
  output += ","; 
  //frint("Br:");
  output += int64String((long long)analogRead(temt6000Pin)); 
  output += ",";
 
  /* --------   presure sensor & termometer   -------- */
  uint32_t rawTemp = ms5611.readRawTemperature();
  uint32_t rawPressure = ms5611.readRawPressure();
  double realTemperature = ms5611.readTemperature();
  long realPressure = ms5611.readPressure();
  float absoluteAltitude = ms5611.getAltitude(realPressure);
  float relativeAltitude = ms5611.getAltitude(realPressure, referencePressure);
 
  //frint("Tb:"); 
  output += int64String((long long)realTemperature);
  output += ",";
 
  //frint("Pb:");
  output += int64String((long long)realPressure);
  output += ",";
 
  //frint("Alb:");
  output += int64String((long long)absoluteAltitude);
  output += ",";
 
  /* --------   termometer 1   -------- */
  //frint("teE:");
  int celsius1 = get_temp(adr1) * 100;
  output += int64String((long long)celsius1 / 100);
  output += ".";
  output += int64String((long long)celsius1 % 100);
  output += ",";
 
  /* --------   termometer 2   -------- */
  //frint("teB:");
  int celsius2 = get_temp(adr2) * 100;
  output += int64String((long long)celsius2 / 100);
  output += ".";
  output += int64String((long long)celsius2 % 100);
  output += ",";
 
  /* --------   termometer 3   -------- */
  //frint("teO:");
  int celsius3 = get_temp(adr3) * 100;
  output += int64String((long long)celsius3 / 100);
  output += ".";
  output += int64String((long long)celsius3 % 100);
  output += ",";
 
  /* --------   current   -------- */
  int current_value = analogRead(A11) * 2;
  double current = (current_value / 37.888) * 1000;
  //frint("cur:");
  output += int64String((long long)current);
  output += ",";
 
  /* --------   voltage   -------- */
  double voltage_value = analogRead(A14) * 2.446950869814567;
  double voltage = voltage_value * 4.8828125;
  //frint("volt:");
  output += int64String((long long)voltage);
  output += ",";
 
  /* --------   heating   -------- */
  if (-10000 < celsius2 && celsius2 < 1000) {
    digitalWrite(11, HIGH);
    heat = 1;
    digitalWrite(heating_lamp, HIGH); // heating indicator
  }
  if (celsius2 <= -10000 || celsius2 >= 2000) {
     digitalWrite(11, LOW);
     heat = 0;
     digitalWrite(heating_lamp, LOW); // heating indicator
  }
  //frint("he:");
  output += int64String((long long)heat);
 
  frintln(output); // printing everything
 
  /* --------   starting termometer 2   -------- */
  start_termo(adr2);
 
  last_print_time = millis(); // time of last printing
}
 
/* --------   main code   -------- */
String output = "";
String Data_conv[3] {"ND", "ND", "ND"}; 
 
void loop() {
  /* --------   parsing data from GPS   -------- */
  parsing();
  if (recievedFlag) {
    Data_conv[0] = Data[0];
    Data_conv[1] = Data[1];
    Data_conv[2] = Data[2];
    upd_time = millis();
    recievedFlag = false;
  }
 
  if (millis() - last_print_time >= 5000) {
    digitalWrite(printing_lamp, HIGH); // printing indicator
    output = "!";
    if (millis() - upd_time > 5500) {
      output += "ND,";
      output += "ND,";
      output += "ND,";
    } else {
      /* --------   converting & pringting data from GPS   -------- */
      if (Data[0] == "WD") {
        output += "WD,";
      } else {
        String sir;
        String sirran;
        for (int y = 0; y < 2; y++) {
          sirran += Data[0][y];
        }
        String sirmin;
        for (int h = 2; h < 10; h++) {
          sirmin += Data[0][h];
        }
        double chissirmin = sirmin.toDouble();
        chissirmin/=60;
        double chissirota = sirran.toDouble() + chissirmin;
        //frint("la:");
 
        long long new_chissirota = chissirota * 1000000;
 
        output += int64String(new_chissirota / 1000000) + "." + int64String(new_chissirota % 1000000);
 
        //Serial.print(chissirota, 6);
        //Serial1.print(chissirota, 6);
        //Serial2.print(chissirota, 6);
 
        //frint(Data[0]);
        output += ",";
      }
 
      if (Data[1] == "WD") {
        output += "WD,";
      } else {
        String dil;
        String dilran;
        for (int i = 0; i < 3; i++) {
          dilran += Data[1][i];
        }
        String dilmin;
        for (int i = 3; i < 11; i++) {
          dilmin += Data[1][i];
        }
        double chisdilmin = dilmin.toDouble();
        chisdilmin /= 60;
        double chisdil = dilran.toDouble() + chisdilmin;
        //frint("lo:");
 
        long long new_chisdil = chisdil * 1000000;
 
        output += int64String(new_chisdil / 1000000) + "." + int64String(new_chisdil % 1000000);
 
        //Serial.print(chisdil, 6);
        //Serial1.print(chisdil, 6);
        //Serial2.print(chisdil, 6);
        output += ",";
      }
 
      /* --------   pringting altitude from GPS   -------- */
      //frint("al:");
      output += Data[2];
      output += ",";
    }
 
    printData(output); // printing rest data
    recievedFlag = false; // set recievedFlag to false
    digitalWrite(printing_lamp, LOW); // printing indicator
  }
 
  /* --------   moving servos   -------- */
  /*
  if (time - last_time >= 1000) {
    //Serial.print(int64String(time - last_time));
    //Serial.print(" ");
    //Serial.println(int64String(time));
    if (servo_dir) {
      myservo.write(servo_pos);
      if (servo_pos >= 175) {
        servo_dir = 0;
      } else {
        servo_pos += 44;
      }
    } else {
      myservo.write(servo_pos);
      if (servo_pos <= 5) {
        servo_dir = 1;
      } else {
        servo_pos -= 44;
      }
    }
    last_time = time;
  }
  */
}
