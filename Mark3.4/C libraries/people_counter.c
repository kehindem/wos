/************************************************************************
 *  @filename: People Counter Source File
 * 	@description: Process a stream of data points delimiting single file 
 * 					and bi-directional movement of people across a threshold, 
 * 					in order to determine the occupancy of an area.
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/

#include "people_counter.h"
#include <stdbool.h>
#include <stdlib.h>

/************************************************************************
 * Private Methods
 ***********************************************************************/
/**
 * @brief Updates the vertical distance measurement by zone 
 * 			for use when detecting midpoint crossings (or peaks).
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param   zone				the zone of interest
 * @param 	distance			the distance measurement 
*/
void addZDistance(people_counter_data* data_ptr, zone_t zone, int distance);

/**
 * @brief Updates the vertical distance measurement by zone 
 * 			for use when detecting midpoint crossings (or peaks).
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param   zone				the zone being queried
 * @return 	true :				peak has occurred in the zone
 * 			false : 			otherwise
*/
bool detectPeopleCounterPeak(people_counter_data* data_ptr, zone_t zone);

/**
 * @brief Obtain the magnitude of the most recent peak distance
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param   zone				the zone being queried
 * @return 	the magnitude of the most recent peak distance
*/
int getPeopleCounterLastPeak(people_counter_data* data_ptr, zone_t zone);

/**
 * @brief Set the status of a zone
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param   zone				the zone being queried
 * @param 	zStatus			 	the zone status
*/
void setZoneStatus(people_counter_data* data_ptr, zone_t zone, zone_status_t zStatus);

/**
 * @brief Initialize the PeopleCounterData storage class
 * 
 * @param 	data_ptr			pointer to people counter data structure
*/
void initializePeopleCounterData(people_counter_data* data_ptr);

/**
 * @brief Clears the PathTrackerData buffer
 * 
 * @param 	data_ptr			pointer to people counter data structure
*/
void clearPathBuffer(people_counter_data* data_ptr);

/**
 * @brief Initialize the PathTrackerData storage class
 * 
 * @param 	data_ptr			pointer to people counter data structure
*/
void initializePathTrackerData(people_counter_data* data_ptr);

/**
 * @brief Update path tracker state variables
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param   zone				the zone being queried
*/
void updatePathTrackerData(people_counter_data* data_ptr, zone_t zone);

/**
 * @brief Process the path tracker information to get the incremental
 * 			change to the people count 
 * 
 * @param 	data_ptr			pointer to people counter data structure
*/
void processPathTrackerData(people_counter_data* data_ptr);

/**
 * @brief Update the PeopleCounterData storage class to track path events
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	zone				the zone being queried
*/
void updatePeopleCounterData(people_counter_data* data_ptr, zone_t zone);

/**
 * @brief Update the people count by processing the tracked zone events in 
 * 			the PeopleCounterData data structure and 
 			peak detection events in the PeakDetection objects.
 * 
 * @param 	data_ptr			pointer to people counter data structure
*/
void updatePeopleCount(people_counter_data* data_ptr);

/**
 * @brief Clear all people counter state variables
 * 
 * @param 	data_ptr			pointer to people counter data structure
*/
void clearPeopleCounterStateVariables(people_counter_data* data_ptr);

/************************************************************************
 * Function Definitions
 ***********************************************************************/
void initializePeopleCounter(people_counter_data* data_ptr, int floorThresh, 
						int floorToCeiling, uint8_t vacancyThresholdPer)
{
	// Configure People Counter Parameters
	data_ptr->parameters.ceilingThreshold = floorThresh - abs(floorToCeiling);
	data_ptr->parameters.floorThreshold = floorThresh;
	data_ptr->parameters.peakVariationTolerance = DEFAULT_PEAK_VARIATION_TOLERANCE;
	setVacancyThreshold(data_ptr, vacancyThresholdPer);

	// Initialize zone status
	data_ptr->leftZoneStatus = VACANT;
	data_ptr->rightZoneStatus = VACANT;

	// Initialize data objects
	initializePeakDetector(&(data_ptr->frontZonePeakDetector), data_ptr->parameters.zoneVacancyThreshold);
	initializePeakDetector(&(data_ptr->backZonePeakDetector), data_ptr->parameters.zoneVacancyThreshold);
	initializePathTrackerData(data_ptr);
	initializePeopleCounterData(data_ptr);
}

zone_status_t getZoneStatus(people_counter_data* data_ptr, zone_t zone)
{
	switch(zone)
	{
		case FRONT:
			return data_ptr->leftZoneStatus;
		case BACK:
			return data_ptr->rightZoneStatus;
	}
}

int getPeopleCount(people_counter_data* data_ptr)
{
	return data_ptr->PplCount;
}

void setPeakDetectorFloor(people_counter_data* data_ptr, int floorValue)
{
	setFloor(&(data_ptr->frontZonePeakDetector), floorValue);
	setFloor(&(data_ptr->backZonePeakDetector), floorValue);
}

void setPeakDetectorWindowSize(people_counter_data* data_ptr, uint8_t window)
{
	setWindowSize(&(data_ptr->frontZonePeakDetector), MIN(window, MAX_WINDOW_SIZE));
	setWindowSize(&(data_ptr->backZonePeakDetector), MIN(window, MAX_WINDOW_SIZE));
}

void setPeakDetectorDispersionTolerance(people_counter_data* data_ptr, uint16_t tol)
{
	setTolerance(&(data_ptr->frontZonePeakDetector), tol);
	setTolerance(&(data_ptr->backZonePeakDetector), tol);
}

void setPeakVariationTolerance(people_counter_data* data_ptr, uint8_t peakVariationTol)
{
	data_ptr->parameters.peakVariationTolerance = peakVariationTol;
}

void setVacancyThreshold(people_counter_data* data_ptr, uint8_t percent)
{
	data_ptr->parameters.zoneVacancyThreshold = 
			data_ptr->parameters.floorThreshold +
			(data_ptr->parameters.ceilingThreshold-data_ptr->parameters.floorThreshold)
				*percent/100;
}

void addData(people_counter_data* data_ptr, int distance, zone_t zone)
{
	if(distance >= data_ptr->parameters.ceilingThreshold)
	{
		if(distance <= data_ptr->parameters.floorThreshold)
		{
			// Zone state update function
			switch(getZoneStatus(data_ptr, zone))
			{
				case VACANT:
          			// No one is around
				 	if(getZoneStatus(data_ptr, FRONT)==VACANT && \
						getZoneStatus(data_ptr, BACK)==VACANT)
					{
						// Clear People Counter data and state variables
						clearPeopleCounterStateVariables(data_ptr);
					}

					if(distance < data_ptr->parameters.zoneVacancyThreshold)
					{
						setZoneStatus(data_ptr, zone, OCCUPIED);
					}
					break;

				case OCCUPIED:
					if(distance >= data_ptr->parameters.zoneVacancyThreshold)
					{
						setZoneStatus(data_ptr, zone, VACANT);
					}
					break;
			}

			// Update peaks information 
			if(getZoneStatus(data_ptr, zone)==OCCUPIED)
			{
				// Add new point to Peak Detector
				addZDistance(data_ptr, zone, distance);

				if(detectPeopleCounterPeak(data_ptr, zone))
				{
					updatePathTrackerData(data_ptr, zone);
				}
			}
			
			// Update PeopleCounter data
			updatePeopleCounterData(data_ptr, zone);

			// Everyone has left
			if(data_ptr->leftZoneStatus==VACANT && \
				data_ptr->rightZoneStatus==VACANT)
			{
				updatePeopleCount(data_ptr);
			}
		}
	}
}

void addZDistance(people_counter_data* data_ptr, zone_t zone, int distance)
{
	switch(zone)
	{
		case FRONT:
			addDistance(&(data_ptr->frontZonePeakDetector), distance);
			break;
		case BACK:
			addDistance(&(data_ptr->backZonePeakDetector), distance);
			break;
	}
}

bool detectPeopleCounterPeak(people_counter_data* data_ptr, zone_t zone)
{
	switch(zone)
	{
		case FRONT:
			return detectPeak(&(data_ptr->frontZonePeakDetector));
		case BACK:
			return detectPeak(&(data_ptr->backZonePeakDetector));
	}
}

int getPeopleCounterLastPeak(people_counter_data* data_ptr, zone_t zone)
{
	switch(zone)
	{
		case FRONT:
			return getLastPeak(&(data_ptr->frontZonePeakDetector));
		case BACK:
			return getLastPeak(&(data_ptr->backZonePeakDetector));
	}
}

void setZoneStatus(people_counter_data* data_ptr, zone_t zone, zone_status_t zStatus)
{ 
	switch(zone)
	{
		case FRONT:
			data_ptr->leftZoneStatus = zStatus;
			break;
		case BACK:
			data_ptr->rightZoneStatus = zStatus;
			break;
	}
}

void initializePeopleCounterData(people_counter_data* data_ptr)
{
	data_ptr->peopleCounterPeaksData.firstPeakZone = NONE;
	data_ptr->peopleCounterPeaksData.lastPeakZone = NONE;
	data_ptr->peopleCounterPeaksData.zoneOverlap = false;
	data_ptr->peopleCounterPeaksData.marginalChange = 0;
	data_ptr->peopleCounterPeaksData.numPeaks = 0;
}

void clearPathBuffer(people_counter_data* data_ptr)
{
	data_ptr->pathTrackerData.window[0].zone = NONE;
	data_ptr->pathTrackerData.window[1].zone = NONE;
	data_ptr->pathTrackerData.bufferCount = 0;
	data_ptr->pathTrackerData.bufferFull = false;
}

void initializePathTrackerData(people_counter_data* data_ptr)
{
	clearPathBuffer(data_ptr);
	data_ptr->pathTrackerData.preWindow.zone = NONE;
}

void updatePathTrackerData(people_counter_data* data_ptr, zone_t zone)
{
	switch(data_ptr->pathTrackerData.bufferCount)
	{
		case 0:
			data_ptr->pathTrackerData.bufferCount++;
			break;
		case 1:
			if(
			(data_ptr->pathTrackerData.window[0].zone==FRONT_ZONE && zone == BACK) 
			|| (data_ptr->pathTrackerData.window[0].zone==BACK_ZONE && zone == FRONT))
			{
				data_ptr->pathTrackerData.bufferFull = true;
				data_ptr->pathTrackerData.preWindow.zone = (zone==FRONT) ? FRONT_ZONE : 
					BACK_ZONE;
				data_ptr->pathTrackerData.preWindow.distance = getPeopleCounterLastPeak(data_ptr, zone);
				data_ptr->pathTrackerData.bufferCount++;
			}	
			else
			{
				data_ptr->pathTrackerData.preWindow.zone = data_ptr->pathTrackerData.\
					window[0].zone;
				data_ptr->pathTrackerData.preWindow.distance = data_ptr->pathTrackerData.\
					window[0].distance;	
			}
			break;
	}

	data_ptr->pathTrackerData.window\
				[data_ptr->pathTrackerData.bufferCount-1].zone = (zone==FRONT) ?
				FRONT_ZONE : BACK_ZONE;

	data_ptr->pathTrackerData.window\
		[data_ptr->pathTrackerData.bufferCount-1].distance = \
			getPeopleCounterLastPeak(data_ptr, zone);
}

void processPathTrackerData(people_counter_data* data_ptr)
{
	if(data_ptr->pathTrackerData.window[0].zone==FRONT_ZONE)
	{
		if(data_ptr->pathTrackerData.window[1].zone==BACK_ZONE)
		{
			if(abs(data_ptr->pathTrackerData.window[0].distance - 
				data_ptr->pathTrackerData.window[1].distance) <= 
				data_ptr->parameters.peakVariationTolerance)
			{
				data_ptr->peopleCounterPeaksData.marginalChange++;
				data_ptr->peopleCounterPeaksData.numPeaks = 
					data_ptr->peopleCounterPeaksData.numPeaks + 2;
			}
			else
			{
				data_ptr->peopleCounterPeaksData.numPeaks = 
					data_ptr->peopleCounterPeaksData.numPeaks + 1;
			}
		}
		else if(data_ptr->pathTrackerData.window[1].zone==NONE)
		{
			if(data_ptr->pathTrackerData.preWindow.zone==BACK_ZONE)
			{
				data_ptr->peopleCounterPeaksData.marginalChange--;
				data_ptr->peopleCounterPeaksData.numPeaks = 
					data_ptr->peopleCounterPeaksData.numPeaks + 1;
			}
		}
	} 
	else if(data_ptr->pathTrackerData.window[0].zone==BACK_ZONE)
	{
		if(data_ptr->pathTrackerData.window[1].zone==FRONT_ZONE)
		{
			if(abs(data_ptr->pathTrackerData.window[0].distance - 
				data_ptr->pathTrackerData.window[1].distance) <= 
				data_ptr->parameters.peakVariationTolerance)
			{
				data_ptr->peopleCounterPeaksData.marginalChange--;
				data_ptr->peopleCounterPeaksData.numPeaks = 
					data_ptr->peopleCounterPeaksData.numPeaks + 2;
			}
			else
			{
				data_ptr->peopleCounterPeaksData.numPeaks = 
					data_ptr->peopleCounterPeaksData.numPeaks + 1;
			}
		}
		else if(data_ptr->pathTrackerData.window[1].zone==NONE)
		{
			if(data_ptr->pathTrackerData.preWindow.zone==FRONT_ZONE)
			{
				data_ptr->peopleCounterPeaksData.marginalChange++;
				data_ptr->peopleCounterPeaksData.numPeaks = 
					data_ptr->peopleCounterPeaksData.numPeaks + 1;
			}
		}
	}
}

void updatePeopleCounterData(people_counter_data* data_ptr, zone_t zone)
{	
	/* First Peak */
	switch(zone)
	{
		case FRONT:
			if(getNumberOfPeaks(&(data_ptr->frontZonePeakDetector))==1 && 
			getNumberOfPeaks(&(data_ptr->backZonePeakDetector))==0)
			{
				data_ptr->peopleCounterPeaksData.firstPeakZone = FRONT_ZONE;
			} 
			break; 
		case BACK:	
			if(getNumberOfPeaks(&(data_ptr->frontZonePeakDetector))==0 && 
			getNumberOfPeaks(&(data_ptr->backZonePeakDetector))==1)
			{
				data_ptr->peopleCounterPeaksData.firstPeakZone = BACK_ZONE;
			}
			break;
	}

	/* Last Peak */
	switch(zone)
	{
		case FRONT:
			if(detectPeak(&(data_ptr->frontZonePeakDetector)))
			{
				data_ptr->peopleCounterPeaksData.lastPeakZone = FRONT_ZONE;
			}
			break; 
		case BACK:	
			if(detectPeak(&(data_ptr->backZonePeakDetector)))
			{
				data_ptr->peopleCounterPeaksData.lastPeakZone = BACK_ZONE;
			}
			break;
	}

	/* Both zones triggered at the same point in time */
	if(data_ptr->leftZoneStatus==OCCUPIED && data_ptr->rightZoneStatus==OCCUPIED)
	{
		data_ptr->peopleCounterPeaksData.zoneOverlap = true;
	}

	/* Update marginal change value */
	if(data_ptr->pathTrackerData.bufferFull)
	{
		processPathTrackerData(data_ptr);
		clearPathBuffer(data_ptr);
	}
}

void updatePeopleCount(people_counter_data* data_ptr)
{
	// Process the last peak if buffer is not full
	if(!data_ptr->pathTrackerData.bufferFull)
	{
		processPathTrackerData(data_ptr);
	}
	

	if(data_ptr->peopleCounterPeaksData.zoneOverlap)
	{
		int multiplier = 1;

		if(data_ptr->peopleCounterPeaksData.firstPeakZone == 
			data_ptr->peopleCounterPeaksData.lastPeakZone)
		{
			// Odd number of peaks
			if(data_ptr->peopleCounterPeaksData.numPeaks % 2 == 1)
			{
				multiplier = -1;
			}	
		}

		data_ptr->PplCount = data_ptr->PplCount + \
			multiplier*data_ptr->peopleCounterPeaksData.marginalChange;
	}
}

void clearPeopleCounterStateVariables(people_counter_data* data_ptr)
{
	initializePathTrackerData(data_ptr);
	initializePeopleCounterData(data_ptr);
	clearPeakDetectorData(&(data_ptr->frontZonePeakDetector));
	clearPeakDetectorData(&(data_ptr->backZonePeakDetector));
}
