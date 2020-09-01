#ifdef ARDUINO  
#include <Arduino.h> // for min function
#else
extern "C" {
#include <stdlib.h>
#include <stdio.h>
}
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#include "PeakDetector.h"

/* Initialize PeakDetector */
PeakDetector::PeakDetector(int floorValue)
{
	this->floorThreshold = floorValue;
	this->window = DEFAULT_WINDOW_DATA_WIDTH;
	this->tolerance = DEFAULT_DISPERSION_TOLERANCE;
	this->reset();
};

/* Set the minimum expected distance value */
void PeakDetector::setFloor(int floorValue) volatile
{
	this->floorThreshold = floorValue;
}

/* Set the sliding window size. Default value is 5 
(defined as DEFAULT_WINDOW_DATA_WIDTH). Maximum value is 
16. */
void PeakDetector::setWindowSize(uint8_t windowSize) volatile
{	
	this->window = (windowSize<=MAX_WINDOW_SIZE) ? windowSize : MAX_WINDOW_SIZE;
}

/* Set the maximum allowance for dispersion in data points from
a true smooth curve. Default tolerance is 10 
(defined as DEFAULT_DISPERSION_TOLERANCE) */
void PeakDetector::setTolerance(uint8_t tol) volatile
{	
	this->tolerance = tol;
}

/* Return the magnitude of the largest peak distance */
int PeakDetector::getLargestPeak(void) volatile
{
	return this->largestPeak;
}

/* Return the number of peaks detected */
int PeakDetector::getNumberOfPeaks(void) volatile
{
	return this->numberofPeaks;
}

/* Return the magnitude of the last peak detected */
int PeakDetector::getLastPeak(void) volatile
{
	return this->lastPeak;
}

/* Return the magnitude of the second largest peak distance */
int PeakDetector::getSecondLastPeak(void) volatile
{
	return this->secondLastPeak;
}

/* Add a distance data point */
void PeakDetector::addDistance(int distance) volatile
{
	if(detectPeak())
	{
		this->bufferCount = 0;
	}

	if(distance<=this->floorThreshold)
	{
		if(this->bufferCount == this->window)
		{
			// shift data set left to add new data  
			for(int i=0; i<this->window-1; i++)
			{
				this->distances[i]=this->distances[i+1];
			}
		}
		else
		{
			// increment buffer index 
			this->bufferCount++;
		}
		// add new data to buffer
		this->distances[this->bufferCount-1] = distance;

		if(detectPeak())
		{
			int peak = this->distances[0];
			for(int i = 0; i < this->window-1; i++)
			{
				peak = (this->distances[i+1]<peak) ? this->distances[i+1] : peak;
			}
			this->secondLastPeak = this->lastPeak;
			this->lastPeak = peak;
			this->largestPeak = min(peak, this->largestPeak);
			this->numberofPeaks++;
		}
	}
}

/* Perform peak detection on the distances in the window */
bool PeakDetector::detectPeak(void) volatile
{
	if(this->bufferCount == this->window)
	{
		// find the slope of the tangent on the left
		int leftSlope = 0;
		for(int i = 1; i <= this->window/2; i++)
		{
			leftSlope = leftSlope + this->distances[i] - this->distances[i-1];
		}

		// find the slope of the tangent on the right
		int rightSlope = 0;
		for(int i = this->window/2+1; i < this->window; i++)
		{
			rightSlope = rightSlope + this->distances[i] - this->distances[i-1];
		}

		// is absolute value of slope close to zero within tolerance?
		int slope = leftSlope + rightSlope;
		if(abs(slope) <= this->tolerance)
		{
			if(leftSlope<0 && rightSlope>0)
			{
				return true;
			}
		}
	}
	return false;
}

/* Reset the peak detector */
void PeakDetector::reset(void) volatile
{
	this->largestPeak = this->floorThreshold;
	this->lastPeak = this->floorThreshold; 
	this->secondLastPeak = this->floorThreshold; 
	this->bufferCount = 0;
	this->numberofPeaks = 0;
	for(int i = 0; i < this->window; i++)
	{
		this->distances[i] = this->floorThreshold;
	}
}
