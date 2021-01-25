/************************************************************************
 *  @filename: Peak Detector Source File 
 * 	@description: Detects the peaks of equidistant points in a moving window
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "peak_detector.h"

void initializePeakDetector(peak_detector_data* data_ptr, int floorValue)
{
	clearPeakDetectorData(data_ptr);
	data_ptr->parameters.floorThreshold = floorValue;
	data_ptr->parameters.window = DEFAULT_WINDOW_DATA_WIDTH;
	data_ptr->parameters.tolerance = DEFAULT_DISPERSION_TOLERANCE;
};

void setFloor(peak_detector_data* data_ptr, int floorValue)
{
	data_ptr->parameters.floorThreshold = floorValue;
}

void setWindowSize(peak_detector_data* data_ptr, uint8_t windowSize)
{	
	data_ptr->parameters.window = (windowSize<=MAX_WINDOW_SIZE) ? windowSize : MAX_WINDOW_SIZE;
}

void setTolerance(peak_detector_data* data_ptr, uint16_t tol)
{	
	data_ptr->parameters.tolerance = tol;
}

int getLargestPeak(peak_detector_data* data_ptr)
{
	return data_ptr->largestPeak;
}

int getNumberOfPeaks(peak_detector_data* data_ptr)
{
	return data_ptr->numberofPeaks;
}

int getLastPeak(peak_detector_data* data_ptr)
{
	return data_ptr->lastPeak;
}

int getSecondLastPeak(peak_detector_data* data_ptr)
{
	return data_ptr->secondLastPeak;
}

void addDistance(peak_detector_data* data_ptr, int distance)
{
	if(detectPeak(data_ptr))
	{
		data_ptr->bufferCount = 0;
	}

	if(distance<=data_ptr->parameters.floorThreshold)
	{
		if(data_ptr->bufferCount == data_ptr->parameters.window)
		{
			// shift data set left to add new data  
			for(int i=0; i<data_ptr->parameters.window-1; i++)
			{
				data_ptr->distances[i]=data_ptr->distances[i+1];
			}
		}
		else
		{
			// increment buffer index 
			data_ptr->bufferCount++;
		}
		// add new data to buffer
		data_ptr->distances[data_ptr->bufferCount-1] = distance;

		if(detectPeak(data_ptr))
		{
			int peak = data_ptr->distances[0];
			for(int i = 0; i < data_ptr->parameters.window-1; i++)
			{
				peak = (data_ptr->distances[i+1]<peak) ? data_ptr->distances[i+1] : peak;
			}
			data_ptr->secondLastPeak = data_ptr->lastPeak;
			data_ptr->lastPeak = peak;
			data_ptr->largestPeak = MIN(peak, data_ptr->largestPeak);
			data_ptr->numberofPeaks++;
		}
	}
}

bool detectPeak(peak_detector_data* data_ptr)
{
	if(data_ptr->bufferCount == data_ptr->parameters.window)
	{
		// find the slope of the tangent on the left
		int leftSlope = 0;
		for(int i = 1; i <= data_ptr->parameters.window/2; i++)
		{
			leftSlope = leftSlope + data_ptr->distances[i] - data_ptr->distances[i-1];
		}

		// find the slope of the tangent on the right
		int rightSlope = 0;
		for(int i = data_ptr->parameters.window/2+1; i < data_ptr->parameters.window; i++)
		{
			rightSlope = rightSlope + data_ptr->distances[i] - data_ptr->distances[i-1];
		}

		// is absolute value of slope close to zero within tolerance?
		int slope = leftSlope + rightSlope;

		if(abs(slope) <= data_ptr->parameters.tolerance)
		{
			if(leftSlope<20 && rightSlope>-20)
			{
				return true;
			}
		}
	}
	return false;
}

void clearPeakDetectorData(peak_detector_data* data_ptr)
{
	data_ptr->largestPeak = data_ptr->parameters.floorThreshold;
	data_ptr->lastPeak = data_ptr->parameters.floorThreshold; 
	data_ptr->secondLastPeak = data_ptr->parameters.floorThreshold; 
	data_ptr->bufferCount = 0;
	data_ptr->numberofPeaks = 0;
	for(int i = 0; i < data_ptr->parameters.window; i++)
	{
		data_ptr->distances[i] = data_ptr->parameters.floorThreshold;
	}
}
