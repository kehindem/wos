/************************************************************************
 *  @filename: People Counter Types
 * 	@description: Type definitions for people counter library
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/

#ifndef PEOPLE_COUNTER_TYPES_H
#define PEOPLE_COUNTER_TYPES_H

#include "zone_types.h"
#include <stdint.h>

typedef enum
{
	FRONT_ZONE,
	BACK_ZONE, 
	NONE
} event_zone_t; 

typedef struct
{
	event_zone_t zone;
	int distance;
} peak_data_t;

typedef struct 
{
	peak_data_t window[2];
	peak_data_t preWindow;
	uint8_t bufferCount;
	bool bufferFull;
} path_tracker_data_t; 

typedef struct
{
	event_zone_t firstPeakZone;
	event_zone_t lastPeakZone;
	bool zoneOverlap;
	uint8_t numPeaks;
	int marginalChange;
} people_counter_peaks_t;

typedef struct 
{
	int zoneVacancyThreshold;
	int ceilingThreshold, floorThreshold;
	uint8_t peakVariationTolerance;
} people_counter_params_t;

#endif
