#ifndef ARDUINO

#include "PeopleCounterTestClass.h"

char PeopleCounterTestClass::ZoneStatusMapping[2][10] = 
{
	"Vacant",
	"Occupied"
};

char PeopleCounterTestClass::EventZoneMapping[3][11] = 
{
	"Left Zone",
	"Right Zone",
	"N/A"
};

PeopleCounterTestClass::PeopleCounterTestClass(int floorThresh, int floorToCeiling, \
	uint8_t vacancyThresholdPer) : PeopleCounter(floorThresh, floorToCeiling,
		 vacancyThresholdPer)
{

}

void PeopleCounterTestClass::print_summary_info()
{
	printf("left zone status: %s\n", this->ZoneStatusMapping[this->getZoneStatus(LEFT)]);
	printf("right zone status: %s\n", this->ZoneStatusMapping[this->getZoneStatus(RIGHT)]);
	printf("people count: %d\n", this->getPeopleCount());
}

void PeopleCounterTestClass::assert_pass_fail(ZoneStatus leftStat, ZoneStatus rightStat, int pplCount)
{
	if(this->getZoneStatus(LEFT)==leftStat && 
		this->getZoneStatus(RIGHT)==rightStat &&
		this->getPeopleCount() == pplCount) 
	{
		this->passedCasesCount++;
		PRINTGREEN("PASS\n");
	}
	else PRINTRED("FAIL\n");
	
	this->testCasesCount++;
}

void PeopleCounterTestClass::print_expected_info(ZoneStatus leftStat, ZoneStatus rightStat, int pplCount)
{
	PRINTYELLOW("Expected: \n");
	printf("left zone %s\n", this->ZoneStatusMapping[leftStat]);
	printf("right zone %s\n", this->ZoneStatusMapping[rightStat]);
	printf("getPeopleCount() returns %d\n", pplCount);
}

void PeopleCounterTestClass::add_data(int* leftData, int* rightData, int length)
{
	int count; 
	int distance;
	for(int i = 0; i<length; i++)
	{
		// left
		count = this->getPeopleCount();

		distance = leftData[i];
		printf("leftDistance: %d\n", distance);
		this->addData(distance, LEFT);

		if(count != this->getPeopleCount())
		{
			this->print_people_counter_data();
		}

		// right
		count = this->getPeopleCount();

		distance = rightData[i];
		printf("rightDistance: %d\n", distance);
		this->addData(distance, RIGHT);

		if(count != this->getPeopleCount())
		{
			this->print_people_counter_data();
		}
	}
}

void PeopleCounterTestClass::add_right_data(int* rightData, int length)
{
	int distance;
	for(int i = 0; i<length; i++)
	{
		distance = rightData[i];
		printf("distance: %d\n", distance);
		this->addData(distance, RIGHT);
	}
}

void PeopleCounterTestClass::add_left_distance(int distance)
{
	printf("distance: %d\n", distance);
	this->addData(distance, LEFT);
}

void PeopleCounterTestClass::add_right_distance(int distance)
{
	printf("distance: %d\n", distance);
	this->addData(distance, RIGHT);
}

void PeopleCounterTestClass::print_people_counter_data()
{
	PRINTYELLOW("-- ZONE INFO --\n");
	printf("num left peaks: %d\n", this->leftZDistances.getNumberOfPeaks());
	printf("num right peaks: %d\n", this->rightZDistances.getNumberOfPeaks());
	printf("zone of first peak: %s\n", \
		this->EventZoneMapping[this->peopleCounterData.firstPeakZone]);
	printf("zone of last peak: %s\n", \
		this->EventZoneMapping[this->peopleCounterData.lastPeakZone]);
	printf("magnitude of incremental change to people counter: %d\n", 
		this->peopleCounterData.marginalChange);
	printf("zone overlap: %d\n", this->peopleCounterData.zoneOverlap);
	printf("adjusted number of peaks: %d\n", this->peopleCounterData.numPeaks);
	PRINTYELLOW("---------\n");
};

void PeopleCounterTestClass::print_test_summary()
{
	PRINTYELLOW("UNIT TEST SUMMARY\n");

	char pass[32], fail[32]; 
	sprintf(pass, "Passed %d / %d test cases.\n\n", \
		this->passedCasesCount, this->testCasesCount);

	sprintf(fail, "Failed %d / %d test cases.\n\n", \
		this->testCasesCount- this->passedCasesCount, 
		this->testCasesCount);

	if(this->testCasesCount == this->passedCasesCount)
	{

		PRINTGREEN(pass);
	}
	else
	{
		PRINTRED(fail);
	}
};

#endif