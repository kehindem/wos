#ifndef ARDUINO
#include <stdio.h>
#include "../PeopleCounter.h"

#define PRINTRED(text){\
					printf("\033[1;31m");\
					printf("%s", text);\
					printf("\033[0m");\
					}

#define PRINTGREEN(text){\
					printf("\033[1;32m");\
					printf("%s", text);\
					printf("\033[0m");\
					}

#define PRINTYELLOW(text){\
					printf("\033[1;33m");\
					printf("%s", text);\
					printf("\033[0m");\
					}

class PeopleCounterTestClass : public PeopleCounter
{
	private:
		static char ZoneStatusMapping[2][10]; 
		static char EventZoneMapping[3][11];
		int testCasesCount;
		int passedCasesCount;
	public: 
		PeopleCounterTestClass(int floorThresh, int floorToCeiling, \
			uint8_t vacancyThresholdPer);
		void print_summary_info();
		void assert_pass_fail(ZoneStatus leftStat, ZoneStatus rightStat, \
			int pplCount);
		void print_expected_info(ZoneStatus leftStat, ZoneStatus rightStat, \
			int pplCount);
		void add_data(int* leftData, int* rightData, int length);
		void add_right_data(int* rightData, int length);
		void add_left_distance(int distance);
		void add_right_distance(int distance);
		void print_people_counter_data();
		void print_test_summary();

};

#endif