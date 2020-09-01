/*
 * People counting software using the laser based VL53L1X
  Authors:   Agiri, Timi
             Bharadwaaj, Sashaank
             Boporai, Rajdeep
             Kehinde, Michael
             Kim, Sue
  Leading Edge Organization
  Email: macleadingedge@gmail.com
  Date: August 27, 2020
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license)

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

// Uncomment the following line to use the developer's feature to verify the software 
// using a sensor emulator instead of the VL53 hardware
#define DEBUG_MODE 

#include <Wire.h>

#ifdef DEBUG_MODE
#include "DEBUG/Mark3.2-Verifier.h"
#include "DEBUG/SensorEmulator.h"
#else
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#endif

#include "src/PeopleCounter/PeopleCounter.h"

//Mandatory interrupt and shutdown pins.
#define SHUTDOWN_PIN 2    // Connect to directly XSHUT pin (pull high to 5V using 10k resistor, if pin on VL53 is too sensitive)
#define INTERRUPT_PIN 3   // Pull high to 5V using 10k resistor and connect to GPIO1 on VL53 

#ifdef DEBUG_MODE
#define DEBUG_PIN 8 // reserved for debugging purposes to generate interrupts. Do not use pin elsewhere
#endif

// ROI Center References
#define LEFT_ROI_REFERENCE    167 //center of left SPAD
#define RIGHT_ROI_REFERENCE   231 //center of right SPAD

// Zone
#define LEFT_ZONE 0
#define RIGHT_ZONE 1

// Interrupt Thresholds
#define DETECTION_THRESHOLD_MAX  1780
#define DETECTION_THRESHOLD_MIN  100

// People Counter Thresholds
#define VACANCY_THRESHOLD_PERCENTAGE 5
#define HEIGHT_TOLERANCE  10 // peak variation tolerance

/* Control Pins */
int LED1 = 6;
int LED2 = 7;

// Time of Flight Sensor Instantiation
#ifdef DEBUG_MODE
SensorEmulator distanceSensor(DEBUG_PIN, 250, 250);
#else
//SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);
#endif

/* Shared variables between interrupt routine and main program */
volatile int currentROIZone;
volatile int distance;
volatile PeopleCounter pplCounter(DETECTION_THRESHOLD_MAX, DETECTION_THRESHOLD_MAX - \
  DETECTION_THRESHOLD_MIN, VACANCY_THRESHOLD_PERCENTAGE);
#ifdef DEBUG_MODE
volatile bool interruptEvent;
volatile int interruptCount;
#endif

/* Interrupt Service Routine for when there is any activity
   in the sensor's field of view */
void getRangingData(void)
{
  #ifdef DEBUG_MODE
  interruptEvent = true;
  interruptCount++;
  #endif
  distance = distanceSensor.getDistance();
  Zone zone = (currentROIZone == LEFT_ZONE) ? LEFT : RIGHT;
  pplCounter.addData(distance, zone);
  distanceSensor.clearInterrupt();
}


//Method to set Region of Interest
void setROICenter(uint16_t center)
{
  uint16_t x = (uint16_t) 8;    //split into 8x16 to use half the sensor
  uint16_t y = (uint16_t) 16;
  distanceSensor.setROI(x, y, center);
}

//Method to initialize sensor, initialize communication
void setup(void)
{
  
  uint16_t X, Y;
  uint8_t Center;

  Wire.begin(); //initialize communication line

  Serial.begin(115200); //baud rate 115200, *(460800 bauds/sec ??)
  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Sensor online!");

  distanceSensor.setDistanceModeShort(); //short mode for vals between 0-1500 mm
  //distanceSensor.setDistanceModeLong();

  Serial.println("Set to Short!");

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  X = distanceSensor.getROIX();
  Y = distanceSensor.getROIY();

  Serial.println("INITIALIZING");
  Serial.print(X);
  Serial.print(",");
  Serial.print(Y);

  Serial.println();

  X = (uint16_t) 8;
  Y = (uint16_t) 16;
  Center = (uint8_t) 167; //sets left spad as initial center

  //print statements for initial setup
  //Serial.print("Setting initial X,Y,Center to %u,%u,%u",X,Y,Center);


  Serial.println("AFTER SETING X Y CENTER");
  Serial.print(X);
  Serial.print(",");
  Serial.print(Y);
  Serial.print(",");
  Serial.print(Center);

  Serial.println();


  distanceSensor.setROI(X, Y, Center);
  //Serial.println("%i,%i,%i have been set.",X,Y,Center);

  Serial.println();
  Serial.print("THE MATRIX: ");
  Serial.print(X);
  Serial.print(",");
  Serial.print(Y);
  Serial.print(",");
  Serial.print(Center);
  Serial.println();

  // Min timing budget for short mode is 20ms; default was 0
  distanceSensor.setTimingBudgetInMs((uint16_t) 50); 
  
  // IMP must be > timing budget + 4ms (default was 103)
  distanceSensor.setIntermeasurementPeriod((uint16_t) 25); 
  
  distanceSensor.setDistanceThreshold(DETECTION_THRESHOLD_MIN, \
                                      DETECTION_THRESHOLD_MAX, (uint8_t) 3);

  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, 1); // drive and keep high 
  
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), getRangingData, FALLING);

  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  
}

void loop(void)
{
  /* LEFT ZONE */
  // wait a couple of milliseconds to ensure the setting of the new ROI center for the next ranging is effective
  // otherwise there is a risk that this setting is applied to current ranging (even if timing has expired, the intermeasurement
  // may expire immediately after.
  delay(500);
  currentROIZone = LEFT_ZONE;
  setROICenter(LEFT_ROI_REFERENCE);
  #ifdef DEBUG_MODE
  distanceSensor.generateInterrupt();
  if(interruptEvent)
  {
    print_debug_information(distance, interruptCount,\
    pplCounter.getZoneStatus(LEFT), pplCounter.getZoneStatus(RIGHT));
    interruptEvent = false;
  }
  #endif


  // Switch to Right ROI
  /* RIGHT ZONE */
  delay(500);
  currentROIZone = RIGHT_ZONE;
  setROICenter(RIGHT_ROI_REFERENCE);
  #ifdef DEBUG_MODE
  distanceSensor.generateInterrupt();
  if(interruptEvent)
  {
    print_debug_information(distance, interruptCount,\
    pplCounter.getZoneStatus(LEFT), pplCounter.getZoneStatus(RIGHT));
    interruptEvent = false;
  }
  #endif


  // No one is around
  if (pplCounter.getZoneStatus(LEFT)==Vacant && pplCounter.getZoneStatus(RIGHT)==Vacant)
  {
    Serial.print("PplCount: ");
    Serial.print(pplCounter.getPeopleCount());
    Serial.println();
    digitalWrite(LED1, 1);
  }
  else
  {
    digitalWrite(LED1, 0);
  }
}

/* Stop Ranging only if we don't want to receive any more data */
//distanceSensor.stopRanging();
