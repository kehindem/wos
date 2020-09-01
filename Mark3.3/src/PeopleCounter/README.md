# People Counter
___

A software library for processing a stream of data points delimiting single file and bi-directional movement of people across a threshold, in order to determine the occupancy of an area. 


[![Leading Edge Logo](https://media-exp1.licdn.com/dms/image/C4E0BAQF-UbPKHHBySg/company-logo_200_200/0?e=2159024400&v=beta&t=gL1TuL_tFEYqSpDxn2hsdtfIJe7isOleo7h-oLenYiE)](https://www.linkedin.com/company/leading-edge-mcmaster)

#### Developer: [Leading Edge](macleadingedge@gmail.com)
#### Author: [Michael Kehinde](kehindem@mcmaster.ca)

## API
### Enumeration Types:
	
>```
Zone:          LEFT, RIGHT
```

The left zone corresponds to the field of view that is closest to the outside. The right zone corresponds to the field of view that is closest to the inside of the area. 



<!-- -->
>```
ZoneStatus:    Vacant, Occupied
```

Vacant - no one is in the zone's field of view.  
Entrance - a person has recently entered the zone's field of view.   
Occupied - a person is still present in the zone's field of view.  
Exit - a person has recently left the zone's field of view. 


### Methods:  


>#### Initialize PeopleCounter.  

>```
PeopleCounter(int floorThresh, int floorToCeiling, 
				    uint8_t vacancyThresholdPer)
```

> *floorThresh:* maximum distance value expected.  

> *floorToCeiling:* expected distance range (difference
 						between the max distance and min distance). Must
 						be greater than 100.
 						 
> *vacancyThresholdPer:* percentage of distance range 
                         specifying a subrange of distances 
                         above the floor in which a zone is 
                         considered vacant. Must be greater than 
                         zero.


<!-- -->
>#### Obtain the zone status.

>```
ZoneStatus getZoneStatus(Zone zone)
```

> *zone:* the zone of interest.  




<!-- -->
>#### Return the number of people counted.

>```
int getPeopleCount(void)
```


<!-- -->
>#### Set the lower threshold for peak detection

>```
void setPeakDetectorFloor(int floor)
```
> *floor:* the lower threshold value. It is generally better to have a floor value as close as possible to your minimum expected peak distance to ensure adequate rejection of lower amplitude peaks that do not correspond to people crossing the zones completely. 



<!-- --> 
>#### Set the sliding window size for peak distance detection.

>```
void setPeakDetectorWindowSize(uint8_t window)
```

> *window:* the window size in number of data points. Window size must be wide enough to encapsulate a single peak but less than the total number of data points. Larger window size is more beneficial for capturing wider peak distance profiles. For narrow peak profiles, peaks close to the end of the data set may be missed if the window size is too large. Default value, defined as DEFAULT\_WINDOW\_DATA\_WIDTH, is 5. Maximum allowable window size is 16.



<!-- -->
>#### Set the dispersion tolerance for detecting a single peak.

> *tol:* maximum allowable deviation of data points within the 
			peak detection window away from a truly smooth curve.
			The default value is 10 (defined as
			DEFAULT\_DISPERSION\_TOLERANCE).
             
>```
void setPeakDetectorDispersionTolerance(uint8_t tol)
```


<!-- -->
>#### Set the peak variation tolerance. 

> *peakVariationTol:* the maximum allowance for variations in a person's peak distance values. The default value is 10 (defined as
DEFAULT\_PEAK\_VARIATION\_TOLERANCE).
             
>```
void setPeakVariationTolerance(uint8_t peakVariationTol)
```



<!-- -->
>#### Set the vacancy threshold. 

> *percent:* percentage of distance range 
             specifying a subrange of distances 
             above the floor in which a zone is 
             considered vacant. Must be greater
             than zero.
             
>```
void setVacancyThreshold(uint8_t percent)
```


<!-- -->
>#### Add a new distance data point from a zone

> *distance:* the distance value. 
 
> *zone:* the zone.

>```
void addData(int distance, Zone zone)
```

## Source Files

### PeopleCounter.h 
Peak Detector Class Declaration.

All methods and variables in the class are qualified as volatile to allow for use in multithreaded programs or use with interrupt routines. 

The library is intended for use on Arduino platform. Compilation for use as a general C++ library is supported. 

### PeopleCounter.cpp
Peak Detector Class Definition.  

Implementation details.

**Developer Note:** The PathTracker storage struct could be implemented as a class to employ better modularity.

## PeopleCounterUnitTests
Verifies the PeopleCounter class library. 