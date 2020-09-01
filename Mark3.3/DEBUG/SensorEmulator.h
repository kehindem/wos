#include <Arduino.h>

#define DEFAULT_ROI_CENTER_REFERENCE 231

class SensorEmulator
{
	private:
		uint16_t ROI;
		int leftCounter;
        int rightCounter;
		int dataLength = 15;
		int leftDistances5[15] = {1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1750, 1780, 1780};
		int rightDistances5[15] = {1780, 1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1750, 1780};
        int debug_pin;
        int timingBudget;
        int interMeasurementPeriod;
    
	public:
		SensorEmulator(int debug_pin, int timingBudget, int interMeasurementPeriod)
		{
          this->debug_pin = debug_pin;
          this->timingBudget = timingBudget;
          this->interMeasurementPeriod = interMeasurementPeriod;
          pinMode(debug_pin, OUTPUT);
          digitalWrite(debug_pin, HIGH);
		}

        
        int begin()
        {
           return 0;
        }

		int getDistance()
		{      
			if(this->ROI==167)
			{
        this->leftCounter++;
				return leftDistances5[min(this->leftCounter-1,dataLength-1)];
			}
			else if(this->ROI==231)
			{
        this->rightCounter++;
				return rightDistances5[min(this->rightCounter-1,dataLength-1)];
			}
		}

		uint16_t getROI()
		{
			return this->ROI;
		}

		void clearInterrupt()
		{
			
		}

        void setROI(int min, int max, int center)
        {
            this->ROI = center;
        }


        void setDistanceModeShort()
        {
          
        }

        void setDistanceModeLong()
        {
          
        }

        uint16_t getROIX()
        {
          return 8;
        }

        uint16_t getROIY()
        {
          return 16;
        }

        void setDistanceThreshold(int minimum, int maximum, int other)
        {
          
        }

        uint16_t getTimingBudgetInMs()
        {
          return 0;
        }

        void setTimingBudgetInMs(uint16_t tb)
        {

        }

        uint16_t getIntermeasurementPeriod()
        {
          
        }

        void setIntermeasurementPeriod(uint16_t imp)
        {
          
        }

        void startRanging()
        {
          
        }

        void stopRanging()
        {
          
        }


        void generateInterrupt()
        {
            if((this->leftCounter!=this->dataLength) \
                && (this->rightCounter!=this->dataLength))
            {
                delay(this->timingBudget);
                digitalWrite(this->debug_pin, LOW);
                delay(this->interMeasurementPeriod);
                digitalWrite(this->debug_pin, HIGH);
            }
        }  
};
