#ifndef PEAKDETECTOR_H
#define PEAKDETECTOR_H

#include <stdint.h>

#define DEFAULT_WINDOW_DATA_WIDTH 5
#define MAX_WINDOW_SIZE 16
#define DEFAULT_DISPERSION_TOLERANCE 10

class PeakDetector
{
	private:	
		volatile int largestPeak;
		volatile int secondLastPeak;
		volatile int distances[MAX_WINDOW_SIZE];
		volatile int numberofPeaks;
		volatile int bufferCount;
		volatile int lastPeak;
		volatile uint8_t tolerance;

		volatile uint8_t window;
		volatile int floorThreshold;

	public: 
		/* detects the peaks of equidistant points with moving window */
		PeakDetector(int floorValue); 
		void setFloor(int floorValue) volatile;
		void setWindowSize(uint8_t windowSize) volatile;
		void setTolerance(uint8_t tol) volatile;
		int getLargestPeak(void) volatile;
		int getNumberOfPeaks(void) volatile;
		int getLastPeak(void) volatile;
		int getSecondLastPeak(void) volatile;
		void addDistance(int distance) volatile;
		bool detectPeak(void) volatile;
		void reset(void) volatile;
};

#endif
