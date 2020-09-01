#People Counter Program

[![Leading Edge Logo](https://media-exp1.licdn.com/dms/image/C4E0BAQF-UbPKHHBySg/company-logo_200_200/0?e=2159024400&v=beta&t=gL1TuL_tFEYqSpDxn2hsdtfIJe7isOleo7h-oLenYiE)](https://www.linkedin.com/company/leading-edge-mcmaster)

###Authors: 
* Agiri, Timi
* Bharadwaaj, Sashaank
* Boporai, Rajdeep
* Kehinde, Michael
* Kim, Sue 

## In This Directory

### Mark3.3.ino 
Main Arduino sketch.
People counting software using the laser based VL53L1X mounted on the threshold of an entry point to a common area. The entry point must accomodate single-file, bi-directional traffic flow. While multiple people may be present in the sensor's field of view, at most one person may cross the threshold at a given point in time. 

### src 
Libraries referenced by the sketch:  

* PeopleCounter
* PeakDetector

### DEBUG
Utilities for software integration testing on the main sketch.

