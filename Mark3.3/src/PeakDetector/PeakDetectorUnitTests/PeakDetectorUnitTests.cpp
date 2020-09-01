#ifndef ARDUINO

#include "../PeakDetector.h"
#include <stdio.h>

#define PRINTRED(text){\
					printf("\033[1;31m");\
					printf(text);\
					printf("\033[0m");\
					}

#define PRINTGREEN(text){\
					printf("\033[1;32m");\
					printf(text);\
					printf("\033[0m");\
					}

#define PRINTYELLOW(text){\
					printf("\033[1;33m");\
					printf(text);\
					printf("\033[0m");\
					}

// Unit Tests for Peak Detector 
PeakDetector pk(1800);

void print_summary_info()
{
	printf("largest peak size: %d\n", pk.getLargestPeak());
	printf("number of peaks detected: %d\n", pk.getNumberOfPeaks());
	printf("last peak: %d\n", pk.getLastPeak());
	printf("second largest peak size: %d\n", pk.getSecondLastPeak());
}

void assert_pass_fail(int largestPeak, int numberOfPeaks, \
	int lastPeak, int secondLastPeak)
{
	if(pk.getLargestPeak()==largestPeak && 
		pk.getSecondLastPeak()==secondLastPeak &&
		pk.getNumberOfPeaks() == numberOfPeaks &&
		pk.getLastPeak() == lastPeak) {PRINTGREEN("PASS\n");}
	else PRINTRED("FAIL\n");
}

void print_expected_info(int largestPeak, int numberOfPeaks, \
	int lastPeak, int secondLastPeak)
{
	printf("Expected: \n");
	printf("  largest peak %d\n", largestPeak);
	printf("  peak count: %d\n", numberOfPeaks);
	printf("  last peak %d\n", lastPeak);
	printf("  second largest peak %d\n", secondLastPeak);
}

void add_data(int dataLength, int* data)
{
	int distance;
	for(int i = 0; i<dataLength; i++)
	{
		distance = data[i];
		pk.addDistance(distance);
		printf("Distance: %d\n", distance);
		printf("Peak?: %d\n\n", pk.detectPeak());
	}
}

int main()
{
	int dataLength;

	/* Verifying the reset function
	pk.reset();
	printf("largest: %d\n", pk.getLargestPeak());
	printf("second largest: %d\n", pk.getSecondLargestPeak());
	printf("numberofpeaks: %d\n", pk.getNumberOfPeaks());
	printf("lastpeak: %d\n", pk.getLastPeak());
	*/

	PRINTYELLOW("-------------TEST CASE 1-----------------\n");
	printf("two mins with window size 5\n");
	print_expected_info(8, 2, 8, 1670);

	pk.reset();
	dataLength = 10;
	int distances1[] = {1700, 1680, 1670, 1680, 1700, 10, 9 , 8, 9, 10};
	add_data(dataLength, distances1);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(8, 2, 8, 1670);
	print_summary_info();
	printf("---------------------------------------\n");




	PRINTYELLOW("-------------TEST CASE 2-----------------\n");
	printf("concave up slope and then decreasing slope; window size 5\n");
	print_expected_info(1800, 0, 1800, 1800);

	pk.reset();
	dataLength = 10;
	int distances2[] = {1700, 1680, 1670, 1660, 1650, 10, 9, 8, 7, 6};
	add_data(dataLength, distances2);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1800, 0, 1800, 1800);
	print_summary_info();
	printf("---------------------------------------\n");





	PRINTYELLOW("-------------TEST CASE 3-----------------\n");
	printf("one min with window size 5\n");
	print_expected_info(1629, 1, 1629, 1800);

	pk.reset();
	dataLength = 10;
	int distances3[] = {1650, 1640, 1635, 1638, 1630, 1629, 1635, 1639, 1645, 1653};
	add_data(dataLength, distances3);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1629, 1, 1629, 1800);
	print_summary_info();
	printf("---------------------------------------\n");




	PRINTYELLOW("-------------TEST CASE 4-----------------\n");
	printf("one min at the allowable tolerance; window size 5\n");
	print_expected_info(1670, 1, 1670, 1800);

	pk.reset();
	dataLength = 5;
	int distances4[] = {1700, 1680, 1670, 1680, 1690};
	pk.setWindowSize(5);
	add_data(dataLength, distances4);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1670, 1, 1670, 1800);
	print_summary_info();
	printf("---------------------------------------\n");




	PRINTYELLOW("-------------TEST CASE 5-----------------\n");
	printf("one min; slope is one value above the allowable tolerance\n");
	print_expected_info(1800, 0, 1800, 1800);

	pk.reset();
	dataLength = 5;
	int distances5[] = {1700, 1680, 1670, 1680, 1689};
	pk.setWindowSize(5);
	add_data(dataLength, distances5);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1800, 0, 1800, 1800);
	print_summary_info();
	printf("---------------------------------------\n");



	PRINTYELLOW("-------------TEST CASE 6-----------------\n");
	printf("straight line increasing slope; window size 5\n");
	print_expected_info(1800, 0, 1800, 1800);

	pk.reset();
	dataLength = 5;
	int distances6[] = {400, 500, 600, 700, 800};
	pk.setWindowSize(5);
	add_data(dataLength, distances6);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1800, 0, 1800, 1800);
	print_summary_info();
	printf("---------------------------------------\n");



	PRINTYELLOW("-------------TEST CASE 7-----------------\n");
	printf("concave down slope\n");
	print_expected_info(1800, 0, 1800, 1800);

	pk.reset();
	dataLength = 5;
	int distances7[] = {400, 500, 600, 500, 400};
	pk.setWindowSize(5);
	add_data(dataLength, distances7);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1800, 0, 1800, 1800);
	print_summary_info();
	printf("---------------------------------------\n");



	PRINTYELLOW("-------------TEST CASE 8-----------------\n");
	printf("one min with window size 10\n");
	print_expected_info(1629, 1, 1629, 1800);

	pk.reset();
	dataLength = 10;
	pk.setWindowSize(10);
	add_data(dataLength, distances3);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1629, 1, 1629, 1800);
	print_summary_info();
	printf("---------------------------------------\n");


	PRINTYELLOW("-------------TEST CASE 8-----------------\n");
	printf("three mins min with window size 5\n");
	print_expected_info(1397, 3, 1398, 1399);

	pk.reset();
	dataLength = 23;
	pk.setWindowSize(5);
	int distances8[] = {1600, 1500, 1397, 1500, 1600, 1780, 1780, 1550, 1500, 1399, 1500, 1550, 1650, 1660, 1600, 1500, 1398, 1500, 1600, 1700, 1750, 1780, 1780};
	add_data(dataLength, distances8);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1397, 3, 1398, 1399);
	print_summary_info();
	printf("---------------------------------------\n");


	PRINTYELLOW("-------------TEST CASE 9-----------------\n");
	printf("two mins min with window size 5\n");
	print_expected_info(1400, 2, 1400, 1670);

	pk.reset();
	dataLength = 14;
	pk.setWindowSize(5);
	int distances9[] = {1700, 1690, 1680, 1670, 1680, 1690, 1600, 1500, 1400, 1500, 1600, 1700, 1750, 1780};
	add_data(dataLength, distances9);

	printf("--------------SUMMARY--------------\n");
	assert_pass_fail(1400, 2, 1400, 1670);
	print_summary_info();
	printf("---------------------------------------\n");
	


}

#endif