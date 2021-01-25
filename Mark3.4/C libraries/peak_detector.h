/************************************************************************
 *  @filename: Peak Detector Header 
 * 	@description: Detects the peaks of equidistant points in a moving window
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/
#ifndef PEAK_DETECTOR_H
#define PEAK_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "peak_detector_types.h"

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

/************************************************************************
 * Data Structure
 ***********************************************************************/
typedef volatile struct
{ 
	// State variables
	int largestPeak;
	int secondLastPeak;
	int distances[MAX_WINDOW_SIZE];
	int numberofPeaks;
	int bufferCount;
	int lastPeak;

	// Parameters
	peak_detector_params_t parameters;
} peak_detector_data;

/************************************************************************
 * Public Methods
 ***********************************************************************/

/**
 * @brief Initialize Peak Detector
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @param   floorValue			maximum expected distance value
 */
void initializePeakDetector(peak_detector_data *data_ptr, int floorValue); 

/**
 * @brief Set the maximum expected distance value.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @param   floorValue			maximum expected distance value
 */
void setFloor(peak_detector_data *data_ptr, int floorValue);

/**
 * @brief Set the sliding window size.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @param   windowSize			the window size in number of data points. 
 * 									Window size must be wide enough to encapsulate 
 * 									a single peak but less than the total number of 
 * 									data points. Larger window sizes are more beneficial 
 * 									for capturing wider peak distance profiles. For 
 * 									narrow peak profiles, peaks close to the end of the 
 * 									data set may be missed if the window size is too large. 
 * 									Default value, defined as DEFAULT_WINDOW_DATA_WIDTH, 
 * 									is 5. Maximum allowable window size is 16.
 */
void setWindowSize(peak_detector_data *data_ptr, uint8_t windowSize);

/**
 * @brief Set the maximum allowance for dispersion in data points.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @param   tol					maximum allowable deviation of data points within 
 * 									the peak detection window away from a truly 
 * 									smooth curve. The default value is 10 
 * 									(defined as DEFAULT_DISPERSION_TOLERANCE).
 */
void setTolerance(peak_detector_data *data_ptr, uint16_t tol);

/**
 * @brief Get the magnitude of the largest peak distance.
 * 
 * @param 	data_ptr				pointer to peak detector data structure
 * @return 	the largest peak distance
 */
int getLargestPeak(peak_detector_data *data_ptr);

/**
 * @brief Get the number of peaks detected.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @return 	the number of peaks detected.
 */
int getNumberOfPeaks(peak_detector_data *data_ptr);

/**
 * @brief Get the magnitude of the last peak detected.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @return 	the magnitude of the last peak detected.
 */
int getLastPeak(peak_detector_data *data_ptr);

/**
 * @brief Get the magnitude of the second last peak detected.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @return 	the magnitude of the second last peak detected.
 */
int getSecondLastPeak(peak_detector_data *data_ptr);

/**
 * @brief Add a distance data point.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @param 	distance			the distance value.
 */
void addDistance(peak_detector_data *data_ptr, int distance);

/**
 * @brief Perform peak detection on the distances in the window.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 * @return 	true 	: 			a peak was detected.
 * 			false 	: 			otherwise. 
 */
bool detectPeak(peak_detector_data *data_ptr);

/**
 * @brief Clear and re-initialize all state variables.
 * 
 * @param 	data_ptr			pointer to peak detector data structure
 */
void clearPeakDetectorData(peak_detector_data *data_ptr);

#endif /* PEAK_DETECTOR_H */
