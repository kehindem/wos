/*
 * People counting software using the laser based VL53L1X
  Authors:   Agiri, Timi
             Bharadwaaj, Sashaank
             Boporai, Rajdeep
             Kehinde, Michael
             Kim, Sue
  Leading Edge Organization
  Email: macleadingedge@gmail.com
  Date: October 28, 2020
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license)

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/



#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "vl53l1x_api.h"
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "people_counter.h"
#include "peak_detector.h"
#include "zone_types.h"

//ROI Center References
#define FRONT_ROI_REFERENCE 167
#define BACK_ROI_REFERENCE 231

//Zone
#define FRONT_ZONE 0
#define BACK_ZONE 1

//Interrupt Thresholds
#define DETECTION_THRESHOLD_MAX 2000
#define DETECTION_THRESHOLD_MIN 0

//People Counter Thresholds
#define VACANCY_THRESHOLD_PERCENTAGE 15
#define HEIGHT_TOLERANCE 10 // peak variation tolerance


// The VL53L1X uses a slightly different way to define the default address of 0x29
// The I2C protocol defintion states that a 7-bit address is used for the device
// The 7-bit address is stored in bit 7:1 of the address register.  Bit 0 is a binary
// value that indicates if a write or read is to occur.  The manufacturer lists the 
// default address as 0x52 (0101 0010).  This is 0x29 (010 1001) with the read/write bit
// alread set to 0.
//uint16_t	dev = 0x29;

uint16_t	dev=0x52;

int status=0;

volatile int IntCount;


void I2C_Init(void);
void UART_Init(void);
void PortG_Init(void);
void VL53L1X_XSHUT(void);

//capture values from VL53L1X for inspection
uint16_t debugArray[100];

#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

#define MAXRETRIES              5           // number of receive attempts before giving up

volatile int currentROIZone;
volatile int distance;
people_counter_data pplCounter;


void I2C_Init(void){
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;           // activate I2C0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;          // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?

    GPIO_PORTB_AFSEL_R |= 0x0C;           // 3) enable alt funct on PB2,3       0b00001100
    GPIO_PORTB_ODR_R |= 0x08;             // 4) enable open drain on PB3 only

    GPIO_PORTB_DEN_R |= 0x0C;             // 5) enable digital I/O on PB2,3
//    GPIO_PORTB_AMSEL_R &= ~0x0C;          // 7) disable analog functionality on PB2,3

                                                                                // 6) configure PB2,3 as I2C
//  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00002200;    //TED
    I2C0_MCR_R = I2C_MCR_MFE;                      // 9) master function enable
    I2C0_MTPR_R = 0b0000000000000101000000000111011;                                        // 8) configure for 100 kbps clock (added 8 clocks of glitch suppression ~50ns)
//    I2C0_MTPR_R = 0x3B;                                        // 8) configure for 100 kbps clock
        
  // 20*(TPR+1)*20ns = 10us, with TPR=24
    // TED 100 KHz
    //     CLK_PRD = 8.3ns
    //    TIMER_PRD = 1
    //    SCL_LP = 6
    //    SCL_HP = 4
    //    10us = 2 * (1 + TIMER_PRD) * (SCL_LP + SCL_HP) * CLK_PRD
    //    10us = 2 * (1+TIMER+PRD) * 10 * 8.3ns
    //  TIMER_PRD = 59 (0x3B)
    //
    // TIMER_PRD is a 6-bit value.  This 0-127
    //    @0: 2 * (1+ 0) * 10 * 8.3ns --> .1667us or 6.0MHz
    //  @127: 2 * (1+ 127) * 10 * 8.3ns --> 47kHz
    
}

//The VL53L1X needs to be reset using XSHUT.  We will use PG0
void PortG_Init(void){
    //Use PortG0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6;                // activate clock for Port N
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R6) == 0){};    // allow time for clock to stabilize
    GPIO_PORTG_DIR_R &= 0x00;                                        // make PG0 in (HiZ)
  GPIO_PORTG_AFSEL_R &= ~0x01;                                     // disable alt funct on PG0
  GPIO_PORTG_DEN_R |= 0x01;                                        // enable digital I/O on PG0
                                                                                                    // configure PG0 as GPIO
  //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
  GPIO_PORTG_AMSEL_R &= ~0x01;                                     // disable analog functionality on PN0

    return;
}

//XSHUT     This pin is an active-low shutdown input; the board pulls it up to VDD to enable the sensor by default. Driving this pin low puts the sensor into hardware standby. This input is not level-shifted.
void VL53L1X_XSHUT(void){
    GPIO_PORTG_DIR_R |= 0x01;                                        // make PG0 out
    GPIO_PORTG_DATA_R &= 0b11111110;                                 //PG0 = 0
    FlashAllLEDs();
    SysTick_Wait10ms(10);
    GPIO_PORTG_DIR_R &= ~0x01;                                            // make PG0 input (HiZ)
		
		return;
}

//device in interrupt mode (GPIO1 pin signal)

#define isInterrupt 1 /* If isInterrupt = 1 then device working in interrupt mode, else device working in polling mode */

void PortL_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;                 //activate the clock for Port L
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R10) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTL_DIR_R &= 0x00;       								   
  GPIO_PORTL_DEN_R |= 0x01;
	GPIO_PORTL_AFSEL_R &= ~0x01;
	GPIO_PORTL_AMSEL_R &= ~0x01; 
	return;
}
//interrupt function

void GPIO1(void){
	GPIO_PORTL_DIR_R |= 0x01; //make PL
	GPIO_PORTL_DATA_R &= 0b00000001;
	
	GPIO_PORTF_DATA_R ^= 0b00010000; 								//hello world!
	SysTick_Wait10ms(1);														//.05s delay
	GPIO_PORTF_DATA_R ^= 0b00010000;
	return;	
}



int main(void) {
	uint8_t byteData, sensorState=0, myByteArray[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , i=0, j=0;
  uint16_t wordData;
 // uint8_t ToFSensor = 1; // 0=Left, 1=Center(default), 2=Right
  uint16_t Distance;
  uint16_t SignalRate;
  uint16_t AmbientRate;
  uint16_t SpadNum; 
  uint8_t RangeStatus;
  uint8_t dataReady;
	
	//initialize
	PLL_Init();
	SysTick_Init();
	onboardLEDs_Init();
	I2C_Init();
	UART_Init();
	PortL_Init();
	
	initializePeopleCounter(&pplCounter, DETECTION_THRESHOLD_MAX, DETECTION_THRESHOLD_MAX - 
		DETECTION_THRESHOLD_MIN, VACANCY_THRESHOLD_PERCENTAGE);
	setPeakDetectorWindowSize(&pplCounter, (uint8_t) 3);
	setPeakDetectorDispersionTolerance(&pplCounter, (uint16_t) 1300);
	setPeakVariationTolerance(&pplCounter, (uint8_t) 100);
	

	
	UART_printf("Program Begins\r\n");
	sprintf(printf_buffer,"Student Occupancy Project\r\n");
	UART_printf(printf_buffer);
	
	status = VL53L1_RdByte(dev, 0x010F, &byteData);					// This is the model ID.  Expected returned value is 0xEA
  myByteArray[i++] = byteData;

  status = VL53L1_RdByte(dev, 0x0110, &byteData);					// This is the module type.  Expected returned value is 0xCC
  myByteArray[i++] = byteData;
	
	status = VL53L1_RdWord(dev, 0x010F, &wordData);
	status = VL53L1X_GetSensorId(dev, &wordData);

	sprintf(printf_buffer,"Model_ID=0x%x , Module_Type=0x%x\r\n",myByteArray[0],myByteArray[1]);
	UART_printf(printf_buffer);
	
	
	//ROI data booting
	#define ROI_CONFIG__USER_ROI_CENTRE_SPAD					0x007F
	uint16_t X;
	uint16_t Y;
	uint16_t x = (uint16_t) 8;    //split into 8x16 to use half the sensor
  uint16_t y = (uint16_t) 16;
	uint8_t FROI = FRONT_ROI_REFERENCE;
	uint8_t BROI = BACK_ROI_REFERENCE;
	
	
	// Booting ToF chip
	while(sensorState==0){
		status = VL53L1X_BootState(dev, &sensorState);
		SysTick_Wait10ms(10);
  }
	FlashAllLEDs();
	UART_printf("ToF Chip Booted!\r\n");
 	UART_printf("One moment...\r\n");
	

  /* This function must to be called to initialize the sensor with the default setting  */
  status = VL53L1X_SensorInit(dev);
	Status_Check("SensorInit", status);
	
  status = VL53L1X_SetDistanceMode(dev, 2); /* 1=short, 2=long */
  status = VL53L1X_SetTimingBudgetInMs(dev, 20); /* in ms possible values [20, 50, 100, 200, 500] */
  status = VL53L1X_SetInterMeasurementInMs(dev, 20); /* in ms, IM must be > = TB */
 // status = VL53L1X_SetDistanceThreshold(dev,DETECTION_THRESHOLD_MIN,DETECTION_THRESHOLD_MAX,3,1);
	status = VL53L1X_SetROI(dev, x, y);
	
	status = VL53L1X_GetROI_XY(dev,&X,&Y);
	sprintf(printf_buffer, "ROI XY = %u, %u\r\n\n",X,Y);
	UART_printf(printf_buffer);
	
	zone_t zone = (currentROIZone == FRONT_ZONE) ? FRONT : BACK;
	
	status = VL53L1X_StartRanging(dev);   /* This function has to be called to enable the ranging */
	Status_Check("StartRanging", status);
	
	
	//for(j=1; j<=60; j++)
	while(1){
	//if the interrupt is raised call the function and get ranging data
	//	while((GPIO_PORTL_DATA_R&0b00000001)==1){
	if(currentROIZone == BACK_ZONE){
			currentROIZone = FRONT_ZONE;
			SysTick_Wait10ms(5);
			status = VL53L1X_SetROICenter(dev, FROI);
			SysTick_Wait10ms(10);
		
			status = VL53L1X_GetROI_XY(dev,&X,&Y);
			status = VL53L1X_GetROICenter(dev,&FROI);
			sprintf(printf_buffer,"ROI XY and Center = %u, %u, %u\r\n",X,Y,FROI);
			//UART_printf(printf_buffer);
		
			FlashLED1(1);
		
			//- loop
			/* BACK ZONE */
			// wait a couple of milliseconds to ensure the setting of the new ROI center for the next ranging is effective
			// otherwise there is a risk that this setting is applied to current ranging (even if timing has expired, the intermeasurement
			// may expire immediately after.
			
	
		while (dataReady == 0){ //while the data is ready start sending data
				status = VL53L1X_CheckForDataReady(dev, &dataReady);
						FlashLED4(1);
						VL53L1_WaitMs(dev, 5);
				}
				
				dataReady = 0;
				status = VL53L1X_GetRangeStatus(dev, &RangeStatus);
				status = VL53L1X_GetDistance(dev, &Distance);
				addData(&pplCounter, Distance, zone);
				debugArray[i] = Distance;
				
				status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
				//status = VL53L1X_GetSignalRate(dev, &SignalRate);
				//status = VL53L1X_GetAmbientRate(dev, &AmbientRate);
				//status = VL53L1X_GetSpadNb(dev, &SpadNum);

				
				sprintf(printf_buffer,"%u,", Distance);
				UART_printf(printf_buffer);
				
				SysTick_Wait10ms(5);
		
  }
	
	if(currentROIZone == FRONT_ZONE) { //THE LAST ITERATION WAS FRONT_ZONE
		currentROIZone = BACK_ZONE;
		SysTick_Wait10ms(5);
		status = VL53L1X_SetROICenter(dev, BROI);
		SysTick_Wait10ms(10);
		status = VL53L1X_GetROI_XY(dev,&X,&Y);
		status = VL53L1X_GetROICenter(dev,&BROI);
		sprintf(printf_buffer,"ROI XY and Center = %u, %u, %u\r\n",X,Y,BROI);
		//UART_printf(printf_buffer);
		
		FlashLED2(1);
		
			//- loop
			/* BACK ZONE */
	
		while (dataReady == 0){ //while the data is ready start sending data
				status = VL53L1X_CheckForDataReady(dev, &dataReady);
						FlashLED4(1);
						VL53L1_WaitMs(dev, 5);
				}
				
				dataReady = 0;
				status = VL53L1X_GetRangeStatus(dev, &RangeStatus);
				status = VL53L1X_GetDistance(dev, &Distance);
				addData(&pplCounter, Distance, zone);
				debugArray[i] = Distance;
				
				status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
				//status = VL53L1X_GetSignalRate(dev, &SignalRate);
				//status = VL53L1X_GetAmbientRate(dev, &AmbientRate);
				//status = VL53L1X_GetSpadNb(dev, &SpadNum);
				sprintf(printf_buffer,"%u\r\n", Distance);
				UART_printf(printf_buffer);
				
				SysTick_Wait10ms(5);
		 
   }

		if(getZoneStatus(&pplCounter,FRONT)==VACANT && getZoneStatus(&pplCounter,BACK)==VACANT){
			sprintf(printf_buffer, "PPLCount : %d\r\n",getPeopleCount(&pplCounter));
			UART_printf(printf_buffer);
			FlashLED3(1);
		}

		
 }//end of while loop
		
	 VL53L1X_StopRanging(dev);
 // }
 			//VL53L1X_StopRanging(dev);

}