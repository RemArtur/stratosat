#include <OneWire.h>
 
OneWire  ds(10);
 
byte adr1[8] = {0x28, 0xFD, 0x6D, 0x41, 0xD, 0x0, 0x0, 0xA4}; 
byte adr2[8] = {0x28, 0x5A, 0x95, 0x3F, 0xD, 0x0, 0x0, 0x58};
byte adr3[8] = {0x28, 0xAA, 0xA, 0x45, 0x51, 0x14, 0x1, 0x8};
 
void start_termo(byte addr[8]) {
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
}
 
float get_temp(byte addr[8]) {
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
 
void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
}
 
//float last_cel1 = 50, last_cel2 = 50;
byte heat = 0;
 
void loop() {
  if (millis() % 1000 == 0) {
    /* --------   starting termometres   -------- */
    start_termo(adr2);
    delay(600);
    start_termo(adr1);
    start_termo(adr3);
 
    /* --------   temp1   -------- */
    Serial.print("teE:");
    int celsius1 = get_temp(adr1) * 100;
    Serial.print(celsius1 / 100);
    Serial.print(".");
    Serial.print(celsius1 % 100);
    Serial.print(",");
 
    /* --------   temp2   -------- */
    Serial.print("teB:");
    int celsius2 = get_temp(adr2) * 100;
    Serial.print(celsius2 / 100);
    Serial.print(".");
    Serial.print(celsius2 % 100);
    Serial.print(",");
 
    /* --------   temp2   -------- */
    Serial.print("teO:");
    int celsius3 = get_temp(adr3) * 100;
    Serial.print(celsius3 / 100);
    Serial.print(".");
    Serial.print(celsius3 % 100);
    Serial.print(",");
 
 
    /* --------   heating   -------- */
    if (-100 < celsius2 && celsius2 < 10) {
      digitalWrite(11, HIGH);
      heat = 1;
    }
    if (celsius2 <= -100 || celsius2 >= 20) {
       digitalWrite(11, LOW);
       heat = 0;
    }
    Serial.print("he:");
    Serial.println(heat);
  }
}
 
//d11 - nagrevatel
//d10 - termo
//28 5A 95 3F D 0 0 58 - батарея 
//28 FD 6D 41 D 0 0 A4 - корпус 
