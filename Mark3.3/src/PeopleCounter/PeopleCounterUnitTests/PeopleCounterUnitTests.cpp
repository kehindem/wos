#ifndef ARDUINO

#include "PeopleCounterTestClass.h"

PeopleCounterTestClass pc(1800, 1600, 5);
int distance;

// Unit Tests for People Counter 
// Inputs: distance, thresholds (vacancy threshold, bad threshold values?)

int main()
{	
	/* State Update Unit Tests */
	
	PRINTYELLOW("-------------TEST CASE i-----------------\n");
	printf("Initialize people counter\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n\n");
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, 0);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");



	PRINTYELLOW("-------------TEST CASE ii-----------------\n");
	printf("Zone vacancy: add right zone data point outside floor threshold\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 1801;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, 0);

	pc.add_left_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE iii-----------------\n");
	printf("Zone vacancy: add right zone data point outside vacancy threshold\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 1721;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, 0);

	pc.add_left_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");



	PRINTYELLOW("-------------TEST CASE iv-----------------\n");
	printf("Zone entrance: add left zone data point within threshold\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 1719;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Occupied, Vacant, 0);

	pc.add_left_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Occupied, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE v-----------------\n");
	printf("Zone occupied: add another left zone data point above floor within threshold\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 1719;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Occupied, Vacant, 0);

	pc.add_left_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Occupied, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE vi-----------------\n");
	printf("Zone exit: add another left zone data point above floor but below vacancy threshold\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 1799;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, 0);

	pc.add_left_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE vii-----------------\n");
	printf("Zone vacant: add another left zone data point above floor but below vacancy threshold\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 1799;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, 0);

	pc.add_left_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE vii-----------------\n");
	printf("Zone vacant: add a right data point above ceiling\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 199;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, 0);

	pc.add_right_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");

	PRINTYELLOW("-------------TEST CASE ix-----------------\n");
	printf("Zone entrance: add a right data point on ceiling\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	distance = 200;
	printf("distance: %d\n\n", distance);
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Occupied, 0);

	pc.add_right_distance(distance);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Occupied, 0);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	

	/* Simulation Tests */
	PRINTYELLOW("-------------TEST CASE 0-----------------\n");
	printf("Right Zone traversal: simulate a traversal across the right zone. Left zone disabled.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int rightDistances0[] = {1780, 1600, 1500, 1510, 1550, 1750, 1800};
	int pplCountAggregate = pc.getPeopleCount() + 0;
	pc.add_right_data(rightDistances0, 7);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 1-----------------\n");
	printf("Entrance through door: simulate a traversal across both zones.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances1[] = {1600, 1500, 1400, 1500, 1600, 1750, 1780, 1790};
	int rightDistances1[] = {1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780};
	//pc.setPeakDetectorFloor(1669); 
	pplCountAggregate = pc.getPeopleCount() + 1;
	pc.add_data(leftDistances1, rightDistances1, 8);
	
	// Pass Fail Criteria
	
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);
	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");
	

	PRINTYELLOW("-------------TEST CASE 2-----------------\n");
	printf("Exit through door: simulate a traversal across both zones.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances2[] = {1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780};
	int rightDistances2[] = {1600, 1500, 1400, 1500, 1600, 1750, 1780, 1790};
	pplCountAggregate = pc.getPeopleCount() - 1;
	pc.add_data(leftDistances2, rightDistances2, 8);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");

	PRINTYELLOW("-------------TEST CASE 3-----------------\n");
	printf("Enter door, forward and back without clearing; without passing right sensor midpoint.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances3[] = {1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1750, 1780};
	int rightDistances3[] = {1780, 1780, 1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780, 1790, 1794, 1799};
	pplCountAggregate = pc.getPeopleCount() + 0;
	pc.add_data(leftDistances3, rightDistances3, 13);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");



	PRINTYELLOW("-------------TEST CASE 4-----------------\n");
	printf("Enter door, forward and back without clearing; but passing right sensor midpoint.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances4[] = {1600, 1500, 1400, 1500, 1600, 1550, 1560, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1780};
	int rightDistances4[] = {1780, 1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1760,1750, 1780, 1780};
	pplCountAggregate = pc.getPeopleCount() + 0;
	pc.add_data(leftDistances4, rightDistances4, 15);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 5-----------------\n");
	printf("Two people enter.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances5[] = {1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1750, 1780, 1780};
	int rightDistances5[] = {1780, 1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1750, 1780};
	pplCountAggregate = pc.getPeopleCount() + 2;
	pc.add_data(leftDistances5, rightDistances5, 15);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");



	PRINTYELLOW("-------------TEST CASE 6-----------------\n");
	printf("Collision at the door.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances6[] = {1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1600, 1500, 1400, 1500, 1600, 1700, 1750, 1780, 1780};
	int rightDistances6[]= {1780, 1780, 1780, 1780, 1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1750, 1780, 1780, 1780, 1780};
	pplCountAggregate = pc.getPeopleCount() - 1;
	pc.add_data(leftDistances6, rightDistances6, 21);

	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");



	PRINTYELLOW("-------------TEST CASE 7-----------------\n");
	printf("Congestion at the door.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances7[] = {1700, 1690, 1680, 1670, 1680, 1690, 1600, 1500, 1400, 1500, 1600, 1700, 1750, 1780};
	int rightDistances7[]= {1700, 1600, 1500, 1400, 1500, 1600, 1700, 1750, 1780, 1780, 1780, 1780, 1780, 1780};
	pplCountAggregate = pc.getPeopleCount() - 1;
	pc.add_data(leftDistances7, rightDistances7, 14);

	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 8-----------------\n");
	printf("Congestion at the door - II.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances8[] = {1600, 1500, 1400, 1500, 1600, 1780, 1780, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1600, 1500, 1400, 1500, 1600, 1700, 1750, 1780, 1780};
	int rightDistances8[]= {1780, 1600, 1500, 1400, 1500, 1600, 1550, 1500, 1400, 1500, 1550, 1650, 1660, 1600, 1500, 1400, 1500, 1600, 1700, 1750, 1780, 1780, 1780};
	pplCountAggregate = pc.getPeopleCount() - 1;
	pc.add_data(leftDistances8, rightDistances8, 23);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 9-----------------\n");
	printf("Two people, opposite sides, step in and back.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances9[] = {1600, 1500, 1400, 1500, 1600, 1750, 1780, 1790};
	int rightDistances9[] = {1780, 1700, 1650, 1600, 1650, 1700, 1750, 1780};
	pplCountAggregate = pc.getPeopleCount() + 0;
	pc.add_data(leftDistances9, rightDistances9, 8);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 10-----------------\n");
	printf("Two people, opposite sides, step in and back with no zone overlap.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances10[] = {1600, 1500, 1400, 1500, 1600, 1750, 1780, 1790, 1790, 1790, 1790, 1790, 1790};
	int rightDistances10[] = {1780, 1780, 1780, 1780, 1780, 1780, 1600, 1500, 1400, 1500, 1600, 1780, 1790};
	pplCountAggregate = pc.getPeopleCount() + 0;
	pc.add_data(leftDistances10, rightDistances10, 13);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 11-----------------\n");
	printf("Mrs. Pacing Palmer: person almost through but kept pacing around sensor field of view before entering fully.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances11[] = {1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780, 1700, 1675, 1700, 1780, 1780, 1700, 1675, 1700, 1780, 1780};
	int rightDistances11[] = {1780, 1780, 1600, 1500, 1400, 1500, 1600, 1780, 1780, 1780, 1780,  1780, 1790, 1790, 1790, 1790, 1790, 1780};
	pplCountAggregate = pc.getPeopleCount() + 1;
	pc.add_data(leftDistances11, rightDistances11, 18);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 12-----------------\n");
	printf("Mr. Pacing Palmer: person almost through but studder stepped before exiting fully.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int rightDistances12[] = {1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780, 1700, 1675, 1700, 1780, 1780, 1780, 1780, 1780, 1780, 1780};
	int leftDistances12[] = {1780, 1780, 1600, 1500, 1400, 1500, 1600, 1780, 1780, 1780, 1780,  1780, 1790, 1790, 1790, 1790, 1790, 1780};
	pplCountAggregate = pc.getPeopleCount() - 1;
	pc.add_data(leftDistances12, rightDistances12, 18);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 13-----------------\n");
	printf("Pacing to and fro across threshold but finishing by entering.\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances13[] = {1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780, 1700, 1675, 1600, 1500, 1400, 1500, 1600, 1700, 1780, 1780, 1780, 1780, 1780, 1780, 1780};
	int rightDistances13[] = {1780, 1780, 1600, 1500, 1400, 1500, 1600, 1600, 1500, 1400, 1500, 1600, 1660, 1600, 1500, 1400, 1500, 1600, 1700, 1790, 1790, 1790, 1780};
	pplCountAggregate = pc.getPeopleCount() + 1;
	pc.add_data(leftDistances13, rightDistances13, 23);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	PRINTYELLOW("-------------TEST CASE 14-----------------\n");
	printf("Contemplative Student Pacing to and fro almost clearing each side. Finish by entering\n");
	printf("ceiling 200, floor 1800, vacancy threshold default 5%%\n");
	int leftDistances14[] = {1780, 1600, 1500, 1400, 1500, 1600, 1750, 1780, 1700, 1675, 1600, 1500, 1400, 1500, 1600, 1700, 1600, 1500, 1400, 1500, 1600, 1780, 1780, 1780, 1780, 1780, 1780, 1780};
	int rightDistances14[] = {1780, 1780, 1600, 1500, 1400, 1500, 1600, 1600, 1500, 1400, 1500, 1600, 1660, 1700, 1780, 1790, 1780, 1780, 1780, 1600, 1500, 1400, 1500, 1600, 1700, 1790, 1790, 1790};
	pplCountAggregate = pc.getPeopleCount() + 1;
	pc.add_data(leftDistances14, rightDistances14, 28);
	
	// Pass Fail Criteria
	pc.print_expected_info(Vacant, Vacant, pplCountAggregate);

	printf("--------------SUMMARY--------------\n");
	pc.assert_pass_fail(Vacant, Vacant, pplCountAggregate);
	pc.print_summary_info();
	PRINTYELLOW("---------------------------------------\n\n");


	pc.print_test_summary();
}

#endif 
