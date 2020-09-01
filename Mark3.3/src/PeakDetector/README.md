# Peak Detector
___

A software library for processing a stream of data points and identifying extreme (minimum) points on a curve. Detects the peak of equidistant points with a moving window. 

[![Leading Edge Logo](https://media-exp1.licdn.com/dms/image/C4E0BAQF-UbPKHHBySg/company-logo_200_200/0?e=2159024400&v=beta&t=gL1TuL_tFEYqSpDxn2hsdtfIJe7isOleo7h-oLenYiE)](https://www.linkedin.com/company/leading-edge-mcmaster)

#### Developer: [Leading Edge](macleadingedge@gmail.com)
#### Author: [Michael Kehinde](kehindem@mcmaster.ca)

## API

### Methods:  

>#### Initialize PeakDetector.  

>```
>PeakDetector(int floorValue)
>```

> *floorValue:* maximum expected distance value.  


<!-- -->
>#### Set the maximum expected distance value.  

>```
>void setFloor(int floorValue)
>```

> *floorValue:* maximum expected distance value. 



<!-- -->
>#### Set the sliding window size.

>```
>void setWindowSize(uint8_t windowSize)
>```

> *windowSize:* the window size in number of data points. Window size must be wide enough to encapsulate a single peak but less than the total number of data points. Larger window sizes are more beneficial for capturing wider peak distance profiles. For narrow peak profiles, peaks close to the end of the data set may be missed if the window size is too large. Default value, defined as DEFAULT\_WINDOW\_DATA\_WIDTH, is 5. Maximum allowable window size is 16.


<!-- -->
>#### Set the maximum allowance for dispersion in data points. 

>```
>void setTolerance(uint8_t tol)
>```

> *tol:* maximum allowable deviation of data points within the 
			peak detection window away from a truly smooth curve.
			The default value is 10 (defined as
			DEFAULT\_DISPERSION\_TOLERANCE).


<!-- -->
>#### Return the magnitude of the largest peak distance. 

>```
>int getLargestPeak(void)
>```


<!-- -->
>#### Return the number of peaks detected. 

>```
>int getNumberOfPeaks(void)
>```


<!-- -->
>#### Return the magnitude of the last peak detected.

>```
>int getLastPeak(void)
>```


<!-- -->
>#### Return the magnitude of the second last peak detected. 

>```
>int getSecondLastPeak(void)
>```


<!-- -->
>#### Add a distance data point.

>```
>void addDistance(int distance)
>```
> *distance:* the distance value. 


<!-- -->
>#### Perform peak detection on the distances in the window.

>```
>bool detectPeak(void)
>```
> Returns true if a peak was detected; false, otherwise.


<!-- -->
>#### Reset Peak Detector

>```
>void reset(void)
>```
>Clears and re-initializes all state variables. 


## Source Files

### PeakDetector.h 
Peak Detector Class Declaration.

All methods and variables in the class are qualified as volatile to allow for use in multithreaded programs or use with interrupt routines. 

The library is intended for use on Arduino platform. Compilation for use as a general C++ library is supported. 

### PeakDetector.cpp
Peak Detector Class Definition.  

Implementation details.

## PeakDetectorUnitTests
Verifies the PeakDetector class library. 