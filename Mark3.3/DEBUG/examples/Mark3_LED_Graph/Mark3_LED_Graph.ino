//Author: Sashaank B
#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3


// ROI Center References
#define LEFT_ROI_REFERENCE		167	//center of left SPAD
#define RIGHT_ROI_REFERENCE		231 //center of right SPAD

SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

//LED
int LED1 = 6;
int LED2 = 7;

//Method to set Region of Interest
void setROICenter(uint16_t center)
{
	uint16_t x, y;
	x = (uint16_t) 8;		//split into 8x16 to use half the sensor
	y = (uint16_t) 16;
	distanceSensor.setROI(x, y, center);
}


//Method to initialize sensor, initialize communication 
void setup(void)
{

	uint16_t X, Y;
	uint8_t Center;
	Wire.begin();	//initialize communication line

	Serial.begin(115200); //baud rate 115200, *(460800 bauds/sec ??)
	Serial.println("VL53L1X Qwiic Test");

	if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
	{
		Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
		while (1);
	}
	Serial.println("Sensor online!");

	distanceSensor.setDistanceModeShort(); //short mode for vals between 0-1500 mm
	//distanceSensor.setDistanceModeLong();

	Serial.println("Set to Short!");

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

	X = distanceSensor.getROIX();
	Y = distanceSensor.getROIY();
/*
	Serial.println("1");
	Serial.print(X);
	Serial.print(",");
	Serial.print(Y);
*/
	Serial.println();

	X = (uint16_t) 8;
	Y = (uint16_t) 16;
	Center = (uint8_t) 167;	//sets left spad as initial center

	//print statements for initial setup
	//Serial.print("Setting initial X,Y,Center to %u,%u,%u",X,Y,Center);
	
	/*
	Serial.println("AFTER SETING X Y CENTER");
	Serial.print(X);
	Serial.print(",");
	Serial.print(Y);
	Serial.print(",");
	Serial.print(Center);

	Serial.println();
	*/
	
	distanceSensor.setROI(X, Y, Center);
	//Serial.println("%i,%i,%i have been set.",X,Y,Center);
	
	X = distanceSensor.getROIX();
	Y = distanceSensor.getROIY();

	//Serial.println("Checking the X,Y,Center: %i,%i,%i.",X,Y,Center);
/*
	Serial.println("AFTER GETING X Y");
	Serial.print(X);
	Serial.print(",");
	Serial.print(Y);

	Serial.println();
	Serial.print("THE MATRIX: ");
	Serial.print(X);
	Serial.print(",");
	Serial.print(Y);
	Serial.print(",");
	Serial.print(Center);
	Serial.println();
*/
}

int getRangingData(void)
{
	distanceSensor.startRanging(); //Write configuration bytes to initiate measurement

	while (!distanceSensor.checkForDataReady())
	{
	delay(1);
	}

	int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
	
	distanceSensor.clearInterrupt();
	distanceSensor.stopRanging();
	return distance;
}

void loop(void)
{
	int minDistance = 100;
	int maxDistance = 1500;
	int leftDistance, rightDistance;

	setROICenter(LEFT_ROI_REFERENCE);
	leftDistance = getRangingData();

	setROICenter(RIGHT_ROI_REFERENCE);
	rightDistance = getRangingData();



	if (minDistance < leftDistance){
	  if (leftDistance <= maxDistance){
		//  Serial.println("Someone is on the left side!");
	  //	Serial.print("Left Distance(mm): ");
	  	Serial.print(leftDistance);
	  //	Serial.println();
    //  digitalWrite(LED1, HIGH);
    //  delay(500);
    //  digitalWrite(LED1, LOW);
	  }
	}
  Serial.print(" ");
	if (minDistance < rightDistance){
	  if (rightDistance <= maxDistance){
		//  Serial.println("Someone is on the right side!");
		//  Serial.print("Right Distance(mm): ");
		  Serial.println(rightDistance);
		//  Serial.println();
    //  digitalWrite(LED2, HIGH);
    //  delay(500);
    //  digitalWrite(LED2, LOW);
	  }
	}

	delay(100);
}
