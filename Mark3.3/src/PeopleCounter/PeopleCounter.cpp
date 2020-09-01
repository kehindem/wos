#include "PeopleCounter.h"

/* PUBLIC METHODS */

// TODO: future dev - make PathTracker into a class with methods
//  	to initialize, clear and check if buffer full

/* 
  Initializer:
 	floorThresh - maximum distance value expected
 	floorToCeiling - expected distance range (i.e. max expected distance - min expected distance)
	vacancyThresholdPer - percentage of distance range closest to the floor in which a zone is considered vacant
*/
PeopleCounter::PeopleCounter(int floorThresh, int floorToCeiling, uint8_t vacancyThresholdPer) : \
		leftZDistances(floorThresh + (floorToCeiling)*vacancyThresholdPer/100), \
		rightZDistances(floorThresh + (floorToCeiling)*vacancyThresholdPer/100)
{
	this->ceilingThreshold = floorThresh - abs(floorToCeiling);
	this->floorThreshold = floorThresh;
	this->peakDetectionFloor = floorThresh;
	this->peakVariationTolerance = DEFAULT_PEAK_VARIATION_TOLERANCE;
	this->setVacancyThreshold(vacancyThresholdPer);
	this->leftZoneStatus = Vacant;
	this->rightZoneStatus = Vacant;
	this->previousLeftZoneStatus = Vacant;
	this->previousRightZoneStatus = Vacant;
	this->clearData();
}

/* 
  Returns the zone status: {Vacant, Occupied}
  	Zone: {LEFT, RIGHT} 
  	*/
ZoneStatus PeopleCounter::getZoneStatus(Zone zone) volatile
{
	switch(zone)
	{
		case LEFT:
			return this->leftZoneStatus;
		case RIGHT:
			return this->rightZoneStatus;
	}
}

// Returns the number of people counted
int PeopleCounter::getPeopleCount(void) volatile
{
	return this->PplCount;
}

/* Set the lower threshold for peak detection */
void PeopleCounter::setPeakDetectorFloor(int floor) volatile
{
	this->peakDetectionFloor = floor;
	this->leftZDistances.setFloor(this->peakDetectionFloor);
	this->rightZDistances.setFloor(this->peakDetectionFloor);
}


/* Set the peak detection window in number of data points 
   Larger window size is more beneficial for wider peaks
   For narrow peaks, window sizes that are too large 
      may miss peaks close to the end of the data set 
   Max window size: 16
*/
void PeopleCounter::setPeakDetectorWindowSize(uint8_t window) volatile
{
	this->leftZDistances.setWindowSize(min(window, MAX_WINDOW_SIZE));
	this->rightZDistances.setWindowSize(min(window, MAX_WINDOW_SIZE));
}

/* Set the peak detection tolerance 
   Higher tolerance allows for a larger dispersion of 
   data points from following a smooth curve
 */
void PeopleCounter::setPeakDetectorDispersionTolerance(uint8_t tol) volatile
{
	this->leftZDistances.setTolerance(tol);
	this->rightZDistances.setTolerance(tol);
}

// Sets the maximum allowance for variations in peak activity for the same person
void PeopleCounter::setPeakVariationTolerance(uint8_t peakVariationTol) volatile
{
	this->peakVariationTolerance = peakVariationTol;
}

/* Set the threshold as a percentage of the detection threshold 
   that a person can be considered to be "in the area"
*/
void PeopleCounter::setVacancyThreshold(uint8_t percent) volatile
{
	this->zoneVacancyThreshold = floorThreshold +
				(ceilingThreshold-floorThreshold)\
				*percent/100;
}

// Add a new distance data point by zone
void PeopleCounter::addData(int distance, Zone zone) volatile
{
	if(distance >= this->ceilingThreshold)
	{
		if(distance <= this->floorThreshold)
		{
			// Save previous zone status
			savePreviousStatus(zone);

			// Zone state update function
			switch(this->getZoneStatus(zone))
			{
				case Vacant:
          			// No one is around
				 	if(this->getZoneStatus(LEFT)==Vacant && \
						this->getZoneStatus(RIGHT)==Vacant)
					{
						// Reset PeopleCounterData
						this->clearData();
					}

					if(distance < this->zoneVacancyThreshold)
					{
						this->setZoneStatus(zone, Occupied);
					}
					break;

				case Occupied:
					if(distance >= this->zoneVacancyThreshold)
					{
						this->setZoneStatus(zone, Vacant);
					}
					break;
			}

			// Update peaks information 
			if(this->getZoneStatus(zone)==Occupied)
			{
				// Add new point to Peak Detector
				this->addZDistance(zone, distance);

				if(this->detectPeak(zone))
				{
					this->updatePathTracker(zone);
				}
			}
			
			// Update PeopleCounter data
			this->updatePeopleCounterData(zone);

			// Everyone has left
			if(this->leftZoneStatus==Vacant && \
				this->rightZoneStatus==Vacant)
			{
				this->processPeopleCounterData();
			}
		}
	}
}

/* PRIVATE METHODS */

/* 
   Updates the vertical distance measurement by zone
   for use when detecting midpoint crossings (or peaks)
*/
void PeopleCounter::addZDistance(Zone zone, int distance) volatile
{
	switch(zone)
	{
		case LEFT:
			this->leftZDistances.addDistance(distance);
			break;
		case RIGHT:
			this->rightZDistances.addDistance(distance);
			break;
	}
}

/* 
   Returns true if a peak has occurred in a zone; 
   false, otherwise.
*/
bool PeopleCounter::detectPeak(Zone zone) volatile
{
	switch(zone)
	{
		case LEFT:
			return this->leftZDistances.detectPeak();
		case RIGHT:
			return this->rightZDistances.detectPeak();
	}
}

// Returns the magnitude of the most recent peak distance
int PeopleCounter::getLastPeak(Zone zone) volatile
{
	switch(zone)
	{
		case LEFT:
			return this->leftZDistances.getLastPeak();
		case RIGHT:
			return this->rightZDistances.getLastPeak();
	}
}

// Sets the status for a zone
void PeopleCounter::setZoneStatus(Zone zone, ZoneStatus zStatus) volatile
{ 
	switch(zone)
	{
		case LEFT:
			this->leftZoneStatus = zStatus;
			break;
		case RIGHT:
			this->rightZoneStatus = zStatus;
			break;
	}
}

// Sets the previous status for a zone
void PeopleCounter::savePreviousStatus(Zone zone) volatile
{ 
	switch(zone)
	{
		case LEFT:
			this->previousLeftZoneStatus = this->leftZoneStatus;
			break;
		case RIGHT:
			this->previousRightZoneStatus = this->rightZoneStatus;
			break;
	}
}

// Initialize the PeopleCounterData storage class
void PeopleCounter::initializePeopleCounterData(void) volatile
{
	this->peopleCounterData.firstPeakZone = NONE;
	this->peopleCounterData.lastPeakZone = NONE;
	this->peopleCounterData.zoneOverlap = false;
	this->peopleCounterData.marginalChange = 0;
	this->peopleCounterData.numPeaks = 0;
}

// Clears the PathTracker buffer
void PeopleCounter::clearPathBuffer(void) volatile
{
	this->pathTracker.window[0].zone = NONE;
	this->pathTracker.window[1].zone = NONE;
	this->pathTracker.bufferCount = 0;
	this->pathTracker.bufferFull = false;
}

// Initialize the PathTracker storage class
void PeopleCounter::initializePathTracker(void) volatile
{
	this->clearPathBuffer();
	this->pathTracker.preWindow.zone = NONE;
}

// Update path tracker state variables
void PeopleCounter::updatePathTracker(Zone zone) volatile
{
	switch(this->pathTracker.bufferCount)
	{
		case 0:
			this->pathTracker.bufferCount++;
			break;
		case 1:
			if(
			(this->pathTracker.window[0].zone==LEFT_ZONE && zone == RIGHT) 
			|| (this->pathTracker.window[0].zone==RIGHT_ZONE && zone == LEFT))
			{
				this->pathTracker.bufferFull = true;
				this->pathTracker.preWindow.zone = (zone==LEFT) ? LEFT_ZONE : 
					RIGHT_ZONE;
				this->pathTracker.preWindow.distance = getLastPeak(zone);
				this->pathTracker.bufferCount++;
			}	
			else
			{
				this->pathTracker.preWindow.zone = this->pathTracker.\
					window[0].zone;
				this->pathTracker.preWindow.distance = this->pathTracker.\
					window[0].distance;	
			}
			break;
	}

	this->pathTracker.window\
				[this->pathTracker.bufferCount-1].zone = (zone==LEFT) ?
				LEFT_ZONE : RIGHT_ZONE;

	this->pathTracker.window\
		[this->pathTracker.bufferCount-1].distance = \
			getLastPeak(zone);
}

// Process the path tracker information to get the incremental
// 	change to the people count 
void PeopleCounter::processPathTracker(void) volatile
{
	if(this->pathTracker.window[0].zone==LEFT_ZONE)
	{
		if(this->pathTracker.window[1].zone==RIGHT_ZONE)
		{
			if(abs(this->pathTracker.window[0].distance - 
				this->pathTracker.window[1].distance) <= 
				this->peakVariationTolerance)
			{
				this->peopleCounterData.marginalChange++;
				this->peopleCounterData.numPeaks = 
					this->peopleCounterData.numPeaks + 2;
			}
			else
			{
				this->peopleCounterData.numPeaks = 
					this->peopleCounterData.numPeaks + 1;
			}
		}
		else if(this->pathTracker.window[1].zone==NONE)
		{
			if(this->pathTracker.preWindow.zone==RIGHT_ZONE)
			{
				this->peopleCounterData.marginalChange--;
				this->peopleCounterData.numPeaks = 
					this->peopleCounterData.numPeaks + 1;
			}
		}
	} 
	else if(this->pathTracker.window[0].zone==RIGHT_ZONE)
	{
		if(this->pathTracker.window[1].zone==LEFT_ZONE)
		{
			if(abs(this->pathTracker.window[0].distance - 
				this->pathTracker.window[1].distance) <= 
				this->peakVariationTolerance)
			{
				this->peopleCounterData.marginalChange--;
				this->peopleCounterData.numPeaks = 
					this->peopleCounterData.numPeaks + 2;
			}
			else
			{
				this->peopleCounterData.numPeaks = 
					this->peopleCounterData.numPeaks + 1;
			}
		}
		else if(this->pathTracker.window[1].zone==NONE)
		{
			if(this->pathTracker.preWindow.zone==LEFT_ZONE)
			{
				this->peopleCounterData.marginalChange++;
				this->peopleCounterData.numPeaks = 
					this->peopleCounterData.numPeaks + 1;
			}
		}
	}
}

// Updates the PeopleCounterData storage class to track path events
void PeopleCounter::updatePeopleCounterData(Zone zone) volatile
{	
	/* First Peak */
	switch(zone)
	{
		case LEFT:
			if(this->leftZDistances.getNumberOfPeaks()==1 && 
			this->rightZDistances.getNumberOfPeaks()==0)
			{
				this->peopleCounterData.firstPeakZone = LEFT_ZONE;
			} 
			break; 
		case RIGHT:	
			if(this->leftZDistances.getNumberOfPeaks()==0 && 
			this->rightZDistances.getNumberOfPeaks()==1)
			{
				this->peopleCounterData.firstPeakZone = RIGHT_ZONE;
			}
			break;
	}

	/* Last Peak */
	switch(zone)
	{
		case LEFT:
			if(this->leftZDistances.detectPeak())
			{
				this->peopleCounterData.lastPeakZone = LEFT_ZONE;
			}
			break; 
		case RIGHT:	
			if(this->rightZDistances.detectPeak())
			{
				this->peopleCounterData.lastPeakZone = RIGHT_ZONE;
			}
			break;
	}

	/* Both zones triggered at the same point in time */
	if(this->leftZoneStatus==Occupied && this->rightZoneStatus==Occupied)
	{
		this->peopleCounterData.zoneOverlap = true;
	}

	/* Update marginal change value */
	if(this->pathTracker.bufferFull)
	{
		this->processPathTracker();
		this->clearPathBuffer();
	}
}

/* Processes tracked zone events in the PeopleCounterData storage object and 
 	peak detection events in the PeakDetection objects to update people count
 	*/
void PeopleCounter::processPeopleCounterData(void) volatile
{
	// Process the last peak if buffer is not full
	if(!this->pathTracker.bufferFull)
	{
		this->processPathTracker();
	}
	

	if(this->peopleCounterData.zoneOverlap)
	{
		int multiplier = 1;

		if(this->peopleCounterData.firstPeakZone == 
			this->peopleCounterData.lastPeakZone)
		{
			// Odd number of peaks
			if(this->peopleCounterData.numPeaks % 2 == 1)
			{
				multiplier = -1;
			}	
		}

		this->PplCount = this->PplCount + \
			multiplier*this->peopleCounterData.marginalChange;
	}
}

/* Reset all data */
void PeopleCounter::clearData(void) volatile
{
	this->initializePathTracker();
	this->initializePeopleCounterData();
	this->leftZDistances.reset();
	this->rightZDistances.reset();
}
