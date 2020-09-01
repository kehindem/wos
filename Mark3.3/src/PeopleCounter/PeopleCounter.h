#ifndef PEOPLECOUNTER_H
#define PEOPLECOUNTER_H

#include "../PeakDetector/PeakDetector.h"
#include <stdint.h>

#ifdef ARDUINO  
#include <Arduino.h> // for abs function
#else
extern "C" {
#include <stdlib.h>
#include <stdio.h>
}
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#define DEFAULT_VACANCY_PERCENTAGE 5
#define DEFAULT_PEAK_VARIATION_TOLERANCE 10

typedef enum 
{
	LEFT,
	RIGHT
} Zone; 

typedef enum {
	Vacant,
	Occupied
} ZoneStatus; 

class PeopleCounter
{
	public:
		PeopleCounter(int floorThresh, int floorToCeiling, \
			uint8_t vacancyThresholdPer);
		ZoneStatus getZoneStatus(Zone zone) volatile;
		int getPeopleCount(void) volatile;
		void setPeakDetectorFloor(int floor) volatile;
		void setPeakDetectorWindowSize(uint8_t window) volatile;
		void setPeakDetectorDispersionTolerance(uint8_t tol) volatile;
		void setPeakVariationTolerance(uint8_t peakVariationTol) volatile;
		void setVacancyThreshold(uint8_t percent) volatile;
		void addData(int distance, Zone zone) volatile; 

	protected:
		//Type definitions
		typedef enum
		{
			LEFT_ZONE,
			RIGHT_ZONE, 
			NONE
		} EventZone; 

		typedef struct
		{
			EventZone zone;
			int distance;
		} PeakData;

		typedef struct 
		{
			PeakData window[2];
			PeakData preWindow;
			uint8_t bufferCount;
			bool bufferFull;
		} PathTracker; 

		typedef struct
		{
			EventZone firstPeakZone;
			EventZone lastPeakZone;
			bool zoneOverlap;
			uint8_t numPeaks;
			int marginalChange;
		} PeopleCounterData;

		// Path Data 
		volatile PathTracker pathTracker;
		volatile PeopleCounterData peopleCounterData;
		volatile PeakDetector leftZDistances;
		volatile PeakDetector rightZDistances;

	private:
		// Parameters
		volatile int zoneVacancyThreshold;
		volatile int ceilingThreshold, floorThreshold;
		volatile uint8_t peakVariationTolerance;
		volatile int peakDetectionFloor;

		// Output
		volatile int PplCount; 

		// Zone Status
		volatile ZoneStatus leftZoneStatus;
		volatile ZoneStatus previousLeftZoneStatus;
		volatile ZoneStatus rightZoneStatus;
		volatile ZoneStatus previousRightZoneStatus;

		// Methods
		void addZDistance(Zone zone, int distance) volatile;
		bool detectPeak(Zone zone) volatile;
		int getLastPeak(Zone zone) volatile;
 		void setZoneStatus(Zone zone, ZoneStatus zStatus) volatile;
		void savePreviousStatus(Zone zone) volatile;
		void initializePeopleCounterData(void) volatile;
		void clearPathBuffer(void) volatile;
		void initializePathTracker(void) volatile;
		void updatePathTracker(Zone zone) volatile;
		void processPathTracker(void) volatile;
		void updatePeopleCounterData(Zone zone) volatile;
		void processPeopleCounterData(void) volatile; 
		void clearData(void) volatile;
};

#endif 
