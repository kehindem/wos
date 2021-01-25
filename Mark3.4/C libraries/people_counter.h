/************************************************************************
 *  @filename: People Counter Header 
 *  @description: Process a stream of data points delimiting single file 
 * 					and bi-directional movement of people across a threshold, 
 * 					in order to determine the occupancy of an area.
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/
#ifndef PEOPLE_COUNTER_H
#define PEOPLE_COUNTER_H

#include "peak_detector.h"
#include "people_counter_types.h"

/************************************************************************
 * Default Tolerance Values
 ***********************************************************************/
#define DEFAULT_VACANCY_PERCENTAGE 5
#define DEFAULT_PEAK_VARIATION_TOLERANCE 10

/************************************************************************
 * Data Structure
 ***********************************************************************/
typedef volatile struct
{
	// State Variables
	path_tracker_data_t pathTrackerData;
	people_counter_peaks_t peopleCounterPeaksData;
	peak_detector_data frontZonePeakDetector;
	peak_detector_data backZonePeakDetector;

	// Parameters
	people_counter_params_t parameters;

	// Output Variable
	int PplCount; 

	// Zone Status
	zone_status_t leftZoneStatus;
	zone_status_t rightZoneStatus;
	zone_status_t previousLeftZoneStatus;
	zone_status_t previousRightZoneStatus;
} people_counter_data;

/************************************************************************
 * Public Methods
 ***********************************************************************/
/**
 * @brief Initialize People Counter
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param   floorThresh			maximum expected distance value
 * @param 	floorToCeiling		expected distance range (difference between the max 
 * 								  	distance and min distance). Must be greater than 100.
 * @param	vacancyThresholdPer percentage of distance range specifying a subrange of 
 * 									distances above the floor in which a zone is 
 * 									considered vacant. Must be greater than zero.
 */
void initializePeopleCounter(people_counter_data* data_ptr, int floorThresh, 
						int floorToCeiling, uint8_t vacancyThresholdPer);

/**
 * @brief Query the people counter for the zone status.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	zone				the zone queried
 * @return	zone_status_t : {VACANT, OCCUPIED}
 */
zone_status_t getZoneStatus(people_counter_data* data_ptr, zone_t zone);

/**
 * @brief Query the people counter for the number of people counted.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @return	the number of people counted.
 */
int getPeopleCount(people_counter_data* data_ptr);

/**
 * @brief Set the lower threshold for peak detection.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	floorValue			the lower threshold value. It is generally 
 * 									more desirable to have a floor value as 
 * 									close as possible to the minimum expected 
 * 									peak distance to ensure adequate rejection 
 * 									of lower amplitude peaks that do not 
 * 									correspond to people crossing the zones 
 * 									completely.
 */
void setPeakDetectorFloor(people_counter_data* data_ptr, int floorValue);

/**
 * @brief Set the sliding window size for peak distance detection.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	window				the window size in number of data points. Window 
 * 									size must be wide enough to encapsulate a 
 * 									single peak but less than the total number of 
 * 									data points. Larger window size is more beneficial 
 * 									for capturing wider peak distance profiles. For 
 * 									narrow peak profiles, peaks close to the end of the 
 * 									data set may be missed if the window size is too large. 
 * 									Default value, defined as DEFAULT_WINDOW_DATA_WIDTH, 
 * 									is 5. Maximum allowable window size is 16.
 */
void setPeakDetectorWindowSize(people_counter_data* data_ptr, uint8_t window);

/**
 * @brief Set the dispersion tolerance for detecting a single peak. Higher tolerance 
 * allows for a larger dispersion of data points from following a smooth curve.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	tol					maximum allowable deviation of data points within the peak 
									detection window away from a truly smooth curve. The 
									default value is 10 (defined as DEFAULT_DISPERSION_TOLERANCE).
 */
void setPeakDetectorDispersionTolerance(people_counter_data* data_ptr, uint16_t tol);

/**
 * @brief Set the peak variation tolerance.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	peakVariationTol	the maximum allowance for variations in a person's peak 
 * 									distance values. The default value is 10 
 * 									(defined as DEFAULT_PEAK_VARIATION_TOLERANCE).
 */
void setPeakVariationTolerance(people_counter_data* data_ptr, uint8_t peakVariationTol);

/**
 * @brief Set the vacancy threshold.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	percent				percentage of distance range specifying a subrange 
 * 									of distances above the floor in which a zone is 
 * 									considered vacant. Must be greater than zero.
 */
void setVacancyThreshold(people_counter_data* data_ptr, uint8_t percent);

/**
 * @brief Add a new distance data point received in a zone.
 * 
 * @param 	data_ptr			pointer to people counter data structure
 * @param 	distance			the distance value
 * @param 	zone				the zone
 */
void addData(people_counter_data* data_ptr, int distance, zone_t zone);

#endif /* PEOPLE_COUNTER_H */