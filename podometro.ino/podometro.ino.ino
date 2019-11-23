
#include "Wire.h"
//#include "I2Cdev.h"
#include <MPU6050_tockn.h>
#include "U8glib.h"
#include "Time.h"
#include "HMC5883L.h"
#include "Adafruit_BMP085.h"
#include "avr/sleep.h"
#include "avr/power.h"
#include "avr/wdt.h"
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

#define NUMCOUNTB1 7
#define NUMCOUNTB2 3
#define NUMCOUNTB3 3
#define NUMCOUNTB4 3
#define NUMBOT1 350 //miliseconds to activate button 2
#define NUM2 7
#define NUM3 10
#define NUM4 7
#define NUM5 33000 //acc. for wlaking
#define NUM6 46000 //acc. for running
#define NUM7 1300 //acc. for running
#define BOTAO 2
#define KcalWalk 0.0392 // == (0,7 m/step) * (56kcal/km) / (1000 m)
#define KcalRun 0.0544 // == (0,8 m/step) * (68kcal/km) / (1000 m)

//BOTOES
int BOTAO_01 = 0;
int BOTAO_02 = 0;
int BOTAO_03 = 0;
int BOTAO_04 = 0;
int BOTAO_05 = 0;
long int timer_botao = 0;

//BUSSOLA
//MPU6050 accelgyro;
Adafruit_BMP085 bmp;
HMC5883L compass;
double error;
int Z = -30;
int k = 0;
int y = 0;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
unsigned long int PdXYZ[NUM3];
unsigned long int PdXYZant;
unsigned long int PdXYZat;
unsigned long int PdXYZmax[NUM4];
unsigned long int PdXYZmed;
unsigned long int timerpasso = 0;
unsigned long int passo = 0;
unsigned long int activestep = 0;
unsigned long int actstep = 0;
boolean andando = false;
boolean correndo = false;
double calorie = 0;
unsigned int KcalD[NUM2];
unsigned int KcalMed;
unsigned long int KcalT = 0;
unsigned int stepD[NUM2];
unsigned int stepMed;
unsigned long int stepT = 0;
boolean dayrefresh = false;
boolean meiopasso = false;
boolean active = false;
unsigned long int timer_var1;
unsigned long int timer_var2;
unsigned long int timer_disp;
unsigned long int timer_step;

String str1;  //declaring string
String str2;  //declaring string
String str3;  //declaring string
String str4;  //declaring string
String str5;  //declaring string
String str6;  //declaring string
String str7;  //declaring string
String str8;  //declaring string
String str9;  //declaring string
String str10;  //declaring string
String str11;  //declaring string
String str12;  //declaring string
String str13;  //declaring string

ISR(WDT_vect)
{
}



void setup() {
  pinMode(BOTAO, INPUT_PULLUP);
  Wire.begin();
 // setTime(18, 28, 0, 21, 06, 2015);
  accelgyro.initialize();
  Wire.beginTransmission(0x68);
  Wire.write(0x37);
  Wire.write(0x02);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x6A);
  Wire.write(0x00);
  Wire.endTransmission();

}

void loop() {
  void pedometer() {
  int m = 0;

  PdXYZ[k] = abs((signed long int) (ax)) + abs((signed long int) (ay)) + abs((signed long int) (az));
  PdXYZant = PdXYZat;
  PdXYZat = PdXYZ[k];

  if (k == (NUM3 - 1)) {
    PdXYZmax[y] = 0;
    for (m = 0; m < NUM3; m++) {
      if (PdXYZmax[y] < PdXYZ[m]) {
        PdXYZmax[y] = PdXYZ[m];
      }
    }
  }
  if (y == (NUM4 - 1)) {
    PdXYZmed = 0;
    for (m = 0; m < NUM4; m++) {
      PdXYZmed = PdXYZmed + PdXYZmax[m] / NUM4;
    }
  }

  if (PdXYZmed > NUM5) {
    if (PdXYZmed > NUM6) {
      correndo = true;
      andando = false;
    } else {
      correndo = false;
      andando = true;
    }
    if ((PdXYZat > (PdXYZant + NUM7)) && (!meiopasso)) {
      meiopasso = true;
      timerpasso = millis();
    }
    if ((PdXYZat < (PdXYZant - NUM7)) && (meiopasso)) {
      activestep++;
      if ((millis() - timer_step) >= 10000) {
        if ((activestep - actstep) >= 10) {
          if (!active) {
            active = true;
            passo = passo + (activestep - actstep);
            calorie = calorie + (correndo * KcalRun + andando * KcalWalk) * (activestep - actstep);
          }
          active = true;
        }
        else {
          active = false;
        }
        actstep = activestep;
        timer_step = millis();
      }
      if (active) {
        passo++;
        calorie = calorie + correndo * KcalRun + andando * KcalWalk;
      }
      meiopasso = false;
    }
  } else {
    correndo = false;
    andando = false;
  }

  if ((millis() - timerpasso) >= 1000) {
    meiopasso = false;
    correndo = false;
    andando = false;
  }

  k++;
  if (k >= NUM3) k = 0;
  y++;
  if (y >= NUM4) y = 0;
}

}
