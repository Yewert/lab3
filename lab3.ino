#include <LedControl.h>
#include <Arduino.h>
#include <MD_TCS230.h>

const int S0 = 30;
const int S1 = 31;
const int S2 = 32;
const int S3 = 33;
MD_TCS230  cs(S2, S3, S0, S1);

const int displaysCount = 1;
const int dataPin = 12;
const int clkPin = 10;
const int csPin = 11;
LedControl lc = LedControl(dataPin, clkPin, csPin, displaysCount);

void setup(){
  Serial.begin(115200);
  Serial.print("hi\n");

  sensorData whiteCalibration;
  whiteCalibration.value[TCS230_RGB_R] = 142750;
  whiteCalibration.value[TCS230_RGB_G] = 90700;
  whiteCalibration.value[TCS230_RGB_B] = 90710;
  sensorData blackCalibration;
  blackCalibration.value[TCS230_RGB_R] = 18550;
  blackCalibration.value[TCS230_RGB_G] = 9600;
  blackCalibration.value[TCS230_RGB_B] = 9600;

  cs.begin();
  cs.setDarkCal(&blackCalibration);
  cs.setWhiteCal(&whiteCalibration);
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 16);
}

void loop(){
  colorData cd;
  readRgb(cd);
  int r = toLedHist(cd.value[TCS230_RGB_R]);
  int g = toLedHist(cd.value[TCS230_RGB_G]);
  int b = toLedHist(cd.value[TCS230_RGB_B]);

  lc.clearDisplay(0);
  printToScreen(0, r);
  printToScreen(2, b);
  printToScreen(4, g);
}

void printToScreen(int col, int val){
  for(int i = 0; i < val; i++){
    lc.setLed(0, col, i, true);  
  }
}

int toLedHist(int value){
  return value / 8;
}

void readRgb(const colorData& cd)
{
    Serial.println("Reading RGB...");
    
    cs.read();
    while (!cs.available())
        ;
    cs.getRGB(&cd);

    printColorData(cd);
}

void callibrate(){
    sensorData blackCalibration, whiteCalibration;

    readRaw(blackCalibration, "BLACK");
    cs.setDarkCal(&blackCalibration);

    readRaw(whiteCalibration, "WHITE");
    cs.setWhiteCal(&whiteCalibration);

    Serial.println("Calibration values:");
    Serial.println();
    printSensorData("whiteCalibration", whiteCalibration);
    printSensorData("blackCalibration", blackCalibration);
}

void readRaw(sensorData& sd, const char* cardColor)
{
    Serial.print("Put ");
    Serial.print(cardColor);
    Serial.println(" card on the sensor and press enter");
    block();

    cs.read();
    while (!cs.available())
        ;
    cs.getRaw(&sd);

    Serial.print(cardColor);
    Serial.println(" calibration finished");
}

void block()
{
    while (!Serial.available())
        ;
    Serial.readStringUntil('\n');
}

void printColorData(const colorData& cd){
    Serial.print(cd.value[TCS230_RGB_R]);
    Serial.print(' ');
    Serial.print(cd.value[TCS230_RGB_G]);
    Serial.print(' ');
    Serial.print(cd.value[TCS230_RGB_B]);
    Serial.println();
}

void printSensorData(const char* name, const sensorData& sd)
{
    Serial.print("sensorData "); Serial.print(name); Serial.println(";");
    printAssignment(name, "R", sd.value[TCS230_RGB_R]);
    printAssignment(name, "G", sd.value[TCS230_RGB_G]);
    printAssignment(name, "B", sd.value[TCS230_RGB_B]);
}

void printAssignment(const char* variableName, const char* color, int32_t value)
{
    Serial.print(variableName);
    Serial.print(".value[TCS230_RGB_");
    Serial.print(color);
    Serial.print("] = ");
    Serial.print(value);
    Serial.println(";");
}
