[![Leading Edge Logo](https://media-exp1.licdn.com/dms/image/C4E0BAQF-UbPKHHBySg/company-logo_200_200/0?e=2159024400&v=beta&t=gL1TuL_tFEYqSpDxn2hsdtfIJe7isOleo7h-oLenYiE)](https://www.linkedin.com/company/leading-edge-mcmaster)

# DEBUG
Provides utilities for software integration testing on the main sketch (../Mark3.2.ino) using the PeopleCounter and PeakDetector class libraries. 


## In This Directory

### SensorEmulator.h
Wrapper software to mimic the VL53L1X time of flight sensor in Arduino sketches.  

The sensor emulator mimics the GPIO1 pin on the VL53 using pin D8 (output mode) on the Arduino.  

*Note:* unlike the VL53, no pull up resistor is required to use the emulator.
Refer to the main sketch ../Mark3.2.ino for examples of its use and operation. 


**Methods:** 

* int begin()
* int getDistance()
* uint16_t getROI()
* void setROI(int min, int max, int center)
* void setDistanceModeShort()
* void setDistanceModeLong()
* uint16_t getROIX()
* uint16_t getROIY()
* void setDistanceThreshold(int minimum, int maximum, int other)
* uint16_t getTimingBudgetInMs()
* uint16_t getIntermeasurementPeriod()
* void startRanging()
* void stopRanging()
* void generateInterrupt()


### Mark3.2-Verifier.h 
Print debug information from the main Arduino sketch. 


### Examples
Example Arduino sketches for debugging.