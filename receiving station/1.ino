#include <LoRa_E32.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// x - синий, y - оранжевый
const int stepX = 2;
const int dirX  = 5;
const int stepY = 3;
const int dirY  = 6;

const int enPin = 8;

int xcur = 0;
int ycur = 0;
int start_delay = 9000;
int base_delay = 60;

void setup() {
  
  Serial.begin(9600);
  
  pinMode(stepX,OUTPUT);
  pinMode(dirX,OUTPUT);


  pinMode(stepY,OUTPUT);
  pinMode(dirY,OUTPUT);


  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);



  digitalWrite(dirX,HIGH);
  digitalWrite(dirY,HIGH);

}


void loop() {
  if (Serial.available()){
    String two = Serial.readString();
    if (two[0] != 'W') return;
    int n = 0;
    int index = 1;
    char mas[3];
    char symb = two[index];
    int n1 = 0;
    int n2 = 0;
    int n3 = 0;
    while(symb != '.'){
      mas[n] = symb;
      n++;
      index++;
      symb = two[index];
    }
    while(symb != '!'){
      index++;
      symb = two[index];
    }
    /*Serial.write("\n");
    Serial.write(mas[0]);
    Serial.write(mas[1]);
    Serial.write(mas[2]);
    */
    if (n == 1){
      n3 = mas[0] - 48;
    }
    if (n == 2){
      n2 = mas[0] - 48;
      n3 = mas[1] - 48;
    }
    if (n == 3){
      n1 = mas[0] - 48;
      n2 = mas[1] - 48;
      n3 = mas[2] - 48;
    }
    long xaxis = n1 * 100 + n2 * 10 + n3;
 
    if (xaxis > 360) return;
 
    int m = 0;
    char mas1[3];
    index++;
    symb = two[index];
    if (symb == '-'){
      index = index + 1;
    }
    int n4 = 0;
    int n5 = 0;
    int n6 = 0;
    symb = two[index];
    while(symb != '.'){
      mas1[m] = symb;
      m++;
      index++;
      symb = two[index];
 
    }
    if (m == 1){
      n6 = mas1[0] - 48;
    }
    if (m == 2){
      n5 = mas1[0] - 48;
      n6 = mas1[1] - 48;
    }
    if (m == 3){
      n4 = mas1[0] - 48;
      n5 = mas1[1] - 48;
      n6 = mas1[2] - 48;
    }
    long yaxis = n4 * 100 + n5 * 10 + n6;
 
    if (yaxis > 90) return;
    
    long xD = abs(xaxis - xcur);
    long yD = abs(yaxis - ycur);
    

    if (xD > 180) xD = 360 - xD;
    
    
    if ((xcur + xD) % 360 == xaxis){
      
      digitalWrite(dirX, LOW);
      xcur += xD;
      xcur = xcur % 360;
      
    } else {
      
      digitalWrite(dirX, HIGH);
      xcur -= xD;
      xcur = xcur % 360;
      
    }


    if ((ycur + yD) == yaxis){
      
      digitalWrite(dirY, HIGH);
      ycur += yD;
      
    } else {
      
      digitalWrite(dirY, LOW);
      ycur -= yD;
      
    }

    
    xD *= 480;
    yD *= 480;

    long half = xD / 2;
    long steps_passed = 0;
    for(int x = 0; x < xD; x++) {
      if (x < half)
      {
        if (base_delay < start_delay)
        {
          start_delay -= 10;
          delayMicroseconds(1000);
          steps_passed += 1;
        }
        if (base_delay == start_delay)
        {
          start_delay -= 1;
        }
        digitalWrite(stepX,HIGH);
        delayMicroseconds(start_delay);
        digitalWrite(stepX,LOW);
        delayMicroseconds(500);
      }
      else
      {
        if (xD - x > steps_passed)
        {
          digitalWrite(stepX,HIGH);
          delayMicroseconds(start_delay);
          digitalWrite(stepX,LOW);
          delayMicroseconds(500);
        }
        else
        {
          start_delay += 1;
          delayMicroseconds(1000);
          digitalWrite(stepX,HIGH);
          delayMicroseconds(start_delay);
          digitalWrite(stepX,LOW);
          delayMicroseconds(500);
        }
     }
  }
  start_delay = 9000;
  delay(500);
  half = yD / 2;
  steps_passed = 0;
 for(int x = 0; x < yD; x++) {
      if (x < half)
      {
        if (base_delay < start_delay)
        {
          start_delay -= 10;
          delayMicroseconds(1000);
          steps_passed += 1;
        }
        if (base_delay == start_delay)
        {
          start_delay -= 1;
        }
        digitalWrite(stepY,HIGH);
        delayMicroseconds(start_delay);
        digitalWrite(stepY,LOW);
        delayMicroseconds(500);
      }
      else
      {
        if (xD - x > steps_passed)
        {
          digitalWrite(stepY,HIGH);
          delayMicroseconds(start_delay);
          digitalWrite(stepY,LOW);
          delayMicroseconds(500);
        }
        else
        {
          start_delay += 1;
          delayMicroseconds(1000);
          digitalWrite(stepY,HIGH);
          delayMicroseconds(start_delay);
          digitalWrite(stepY,LOW);
          delayMicroseconds(500);
        }
     }
  }
   start_delay = 9000;
   delay(500);
  }
}
