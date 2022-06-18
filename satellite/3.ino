#include <Int64String.h> // int ot string
 
/* --------   parsing data from GPS   -------- */
String Data[3] {"WD", "WD", "WD"}; 
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
 
void frint(String output) {
  Serial.print(output);
  Serial1.print(output);
  Serial2.print(output);
}
void frintln(String output) {
  Serial.println(output);
  Serial1.println(output);
  Serial2.println(output);
}
 
void setup() {
  Serial.begin(57600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}
 
long long t = 0;
long long last_t = 0;
long long upd_t = 0;
 
String Data_conv[3] {"---", "---", "---"}; 
 
void loop() {
  parsing();       // функция парсинга
  t = millis();
  if (t - last_t == 1000) {
    if (t - upd_t > 1100) {
      frint("ND,");
      frint("ND,");
      frint("ND,");
      frint(int64String(t / 1000));
      frintln("");
      last_t = t;
    } else {
      frint(Data_conv[0] + ",");
      frint(Data_conv[1] + ",");
      frint(Data_conv[2] + ",");
      frint(int64String(t / 1000));
      frintln("");
      last_t = t;
    }
  }
  if (recievedFlag) {
    Data_conv[0] = Data[0];
    Data_conv[1] = Data[1];
    Data_conv[2] = Data[2];
    upd_t = millis();
    recievedFlag = false;
  }
}
