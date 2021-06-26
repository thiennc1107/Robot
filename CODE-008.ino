#define enA  11
#define in1 A0
#define in2 A1
#define in3 A2
#define in4 A3
#define enB 10
#define phase_a2 3  //interrupt1
#define phase_b2 7
#define phase_a1 2  //interrupt0
#define phase_b1 5
const int interrupt0 = 0; //pin2
const int interrupt1 = 1; //pin3
#define CTHT1 8
#define CTHT2 9
char Input;
#include <EEPROM.h>
#include <Servo.h>
Servo myservo;
int pos = 90;            //goc quay servo
int agl1, agl2, aglf1, aglf2;          //goc quay motor
int pulse1 =  0;//EEPROM.read(0) * 255 +  EEPROM.read(1); //xung encoder1
int pulse2 =  0;//EEPROM.read(2) * 255 +  EEPROM.read(3); //xung encoder2
int speedd = 2000;
float c1, c2, a1, a2, d1, d2, Px, Py, t1, t2;
float pi = 3.141592654;
//...................................................
void setup() {
  Serial.begin(9600);
  myservo.attach(6);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(phase_a1, INPUT_PULLUP);
  pinMode(phase_b1, INPUT_PULLUP);
  pinMode(phase_a2, INPUT_PULLUP);
  pinMode(phase_b2, INPUT_PULLUP);
  pinMode(CTHT1, INPUT_PULLUP);
  pinMode(CTHT2, INPUT_PULLUP);
  attachInterrupt(interrupt0, int1_, FALLING);
  attachInterrupt(interrupt1, int2_, FALLING);
  myservo.write(0);
}
//...................................................
void int1_() {
  if (digitalRead(phase_b1) == LOW) {
    pulse1--;
  }
  else {
    pulse1++;
  }
}
//...................................................
void int2_() {
  if (digitalRead(phase_b2) == LOW) {
    pulse2--;
  }
  else {
    pulse2++;
  }
}
//---------------------------------------------------
void loop()
{
  SetHome();
  Serial.print(pulse1);
  Serial.print(", ");
  Serial.println(pulse2);
  Read_serial();
  if ((agl1 != 0) && (agl2 != 0)) {
    runn();
  }
}
//---------------------------------------------------
void Fw1() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 255);
  delayMicroseconds(speedd);
  analogWrite(enA, 0);
  delayMicroseconds(speedd);
  //Serial.println(pulse1);
}
//...................................................
void Fw2() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 255);
  delayMicroseconds(speedd);
  analogWrite(enB, 0);
  delayMicroseconds(speedd);
  //Serial.println(pulse2);
}
//...................................................
void Rv1() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 255);
  delayMicroseconds(speedd);
  analogWrite(enA, 0);
  delayMicroseconds(speedd);
  //Serial.println(pulse1);
}
//...................................................
void Rv2() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 255);
  delayMicroseconds(speedd);
  analogWrite(enB, 0);
  delayMicroseconds(speedd);
  //Serial.println(pulse2);
}
//...................................................
void Stp1() {
  analogWrite(enA, 0);
  //Serial.println(pulse2);
}
//...................................................
void Stp2() {
  analogWrite(enB, 0);
  //Serial.println(pulse2);
}
//...................................................
void SetHome() {
  while ((pulse1 != 0) || (pulse2 != 0)) {
    if (pulse1 > 0) {
      Rv1();
    }
    else if (pulse1 < 0) {
      Fw1();
    }
    if (pulse2 > 0) {
      Rv2();
    }
    else if (pulse2 < 0) {
      Fw2();
    }
  }
}
//...................................................
void Save() {
  EEPROM.write(0, pulse1 / 255);
  EEPROM.write(1, pulse1 % 255);
  EEPROM.write(2, pulse2 / 255);
  EEPROM.write(3, pulse2 % 255);
}
//...................................................
void pen() {
  Serial.println("pen down");
  myservo.write(pos);
  delay(1000);
  Serial.println("pen up");
  myservo.write(0);
  delay(700);
}
//...................................................
void Read_serial() {
  while (Serial.available() > 0) {
    Input = (char)Serial.read();
    Serial.print((char)Input);
    if (Input == '1') {
      Px = -100; Py = 450;
    }
    else if (Input == '2') {
      Px = 0; Py = 450;
    }
    else if (Input == '3') {
      Px = 100; Py = 450;
    }
    else if (Input == '4') {
      Px = -100; Py = 345;
    }
    else if (Input == '5') {
      Px = 0; Py = 345;
    }
    else if (Input == '6') {
      Px = 100; Py = 345;
    }
    else if (Input == '7') {
      Px = -100; Py = 245;
    }
    else if (Input == '8') {
      Px = 0; Py = 245;
    }
    else if (Input == '9') {
      Px = 100; Py = 245;
    }
    else if (Input == 'L') { //l1
      pulse1 = 0;
    }
    else if (Input == 'R') { //r1
      pulse1 = 0;
    }
    else if (Input == 'l') { //l2
      pulse2 = 0;
    }
    else if (Input == 'r') { //r2
      pulse2 = 0;
    }
    solve();
    agl1 = map(t1, 0, 360, 0, 2950);
    agl2 = map(t2, 0, 360, 0, 2950);
    aglf1 = agl1 - 1404;
    aglf2 = agl2 - 72;
    //        Serial.println();
    //        Serial.println(aglf2);
    //        while (1) {}
  }
}
//...................................................
void runn() {
  unsigned long int timer1;
  timer1 = millis();
  while (millis() - timer1 < 4000) {
    if (aglf1 > 0) {
      if (pulse1 < -abs(aglf1)) {
        Fw1();
      }
      else if (pulse1 > -abs(aglf1)) {
        Rv1();
      }
    }
    else if (aglf1 < 0) {
      if (pulse1 < abs(aglf1)) {
        Fw1();
      }
      else if (pulse1 > abs(aglf1)) {
        Rv1();
      }
    }
    //0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0
    if (aglf2 > 0) {
      if (pulse2 > -abs(aglf2)) {
        Rv2();
      }
      else if (pulse2 < -abs(aglf2)) {
        Fw2();
      }
    }
    else if (aglf2 < 0) {
      if (pulse2 < abs(aglf2)) {
        Fw2();
      }
      else if (pulse2 > abs(aglf2)) {
        Rv2();
      }
    }
    Serial.println(abs(pulse2 - abs(aglf2)));
  }
  pen();
  agl1 = 0;
  agl2 = 0;
}
//...................................................
void solve() {
  c1 = sqrt(Py * Py + (75 + Px) * (75 + Px));
  c2 = sqrt(Py * Py + (75 - Px) * (75 - Px));
  a2 = acos((-39375 + c2 * c2) / (2 * 225 * c2));
  a1 = acos((-39375 + c1 * c1) / (2 * 225 * c1));
  d2 = acos((21600 - 300 * Px) / (2 * 147 * c2));
  d1 = acos((21600 + 300 * Px) / (2 * 147 * c1));
  t2 = (pi - a2 - d2) / pi * 180;
  t1 = (a1 + d1) / pi * 180;
}
