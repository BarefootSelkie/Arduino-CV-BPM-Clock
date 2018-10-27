#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Timer.h"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

int pinBPM = 0; // select the input pin for the potentiometer
int pinSwing = 1;
int pinBar = 6;
int pinBeat = 9;
int pinQuaver = 10;
int pinSemi = 11;

int setBar = 4;
int setBeats = 4;
int setWidth = 20;
int setSwing = 0;

float lastBPM = 0;
float lastSwing = 0;

const long minute = 60000;

Timer t;

int currentBeat = 0;

// This function turns all of the LEDs back off again.
void dim(void* context)
{
  digitalWrite(pinBar, LOW);
  digitalWrite(pinBeat, LOW);
  digitalWrite(pinQuaver, LOW);
  digitalWrite(pinSemi, LOW);
}

// This function runs on every beat
void onBeat(void* context)
{
  // Check if we're on the first beat of the bar
  if (currentBeat % (4 * setBar)  == 0)
  {
    digitalWrite(pinBar, HIGH);
    currentBeat = 0;
  }
  if (currentBeat % 4 == 0)
  {
    digitalWrite(pinBeat, HIGH);
  }
  if (currentBeat % 2 == 0)
  {
    digitalWrite(pinQuaver, HIGH);
  }
  
  if (currentBeat % 2 == 0)
  {
    digitalWrite(pinSemi, HIGH);

    // Rerun this code in a short while based on the current bpm
    int interval = minute / (getBPM() * 4);
    interval = interval + ((getSwing() * interval) / 200); // Apply swing
    t.after(interval, onBeat, (void*)10);
  }
  else
  {
    
    digitalWrite(pinSemi, HIGH);
    
    // Rerun this code in a short while based on the current bpm
    int interval = minute / (getBPM() * 4);
    interval = interval - ((getSwing() * interval) / 200); // Apply swing
    t.after(interval, onBeat, (void*)10);
  }

  // Update the counter that indicates which beat of the bar we're on
  currentBeat++;  

  // Dim the LEDs again after 100ms
  t.after(setWidth, dim, (void*)10);


}

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  
  pinMode(6, OUTPUT); 
  pinMode(9, OUTPUT); 
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT); 
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("Hello");
  display.setCursor(10,16);
  display.println("World");
  display.display();

  long test = 100;
  Serial.println("started");

  digitalWrite(9, HIGH);

  t.after(100, onBeat, (void*)10);
}

void loop() {
  // Ask the timer library to check if anything needs to happen yet, and do it if so.
  t.update();

  // Update the OLED display to show information
  display.setCursor(5,5);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);

  display.println(getBPM());
  
  display.setCursor(80,5);
  display.setTextSize(1);
  display.print(setBar);
  display.print(" / ");
  display.println(setBeats);

  display.setCursor(80,15);
  display.setTextSize(1);
  display.print(setWidth);
  display.print("ms");
  
  display.setCursor(80,25);
  display.setTextSize(1);
  display.print(getSwing());
  display.println(" %");
  
  display.display();
}

// Return a BPM based on the current value of the potentiometer
int getBPM()
{
  int potReading = 0;       // variable to store the value coming from the sensor
  int outValue = 0;
  float newValue = 0;
  float smoothingAmount = 0.1;
  
  potReading = analogRead(pinBPM);

  newValue = (smoothingAmount * potReading) + ((1-smoothingAmount) * lastBPM);

  lastBPM = newValue;

  outValue = newValue;

  return ( outValue / 4) + 1;
}

int getSwing()
{
  int potReading = 0;       // variable to store the value coming from the sensor
  int outValue = 0;
  float newValue = 0;
  float smoothingAmount = 0.2;
  
  potReading = analogRead(pinSwing);

  newValue = (smoothingAmount * potReading) + ((1-smoothingAmount) * lastSwing);

  lastSwing = newValue;

  newValue = (newValue / 1024) * 101; // As scale is 0 - 100 it needs to be divided by 101
  
  outValue = newValue;

  return ( outValue );
}
