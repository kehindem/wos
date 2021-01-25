/************************************************************************
 *  @filename: Peak Detector Types 
 * 	@description: Type definitions for peak detector parameters and state variables
 *  @author Michael Kehinde <kehindem@mcmaster.ca>
 *  @date October 14, 2020
 ***********************************************************************/
#ifndef PEAK_DETECTOR_TYPES_H
#define PEAK_DETECTOR_TYPES_H

/************************************************************************
 * Default Parameter Values
 ***********************************************************************/
#define DEFAULT_WINDOW_DATA_WIDTH 5
#define MAX_WINDOW_SIZE 16
#define DEFAULT_DISPERSION_TOLERANCE 900

#include <stdint.h>

typedef volatile struct
{
	uint16_t tolerance;
	uint8_t window;
	int floorThreshold;
} peak_detector_params_t; 

#endif /* PEAK_DETECTOR_TYPES_H */
