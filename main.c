#include <MKL46Z4.h>
#include <utils.h>
#include <stdint.h>
#include <dataProcessing.h>
#include <dataQueue.h>

#include <stdio.h>
#include <pin_mux.h>
#include <clock_config.h>
#include <board.h>
#include <fsl_debug_console.h>

#define LIGHT_SENSOR_PIN (22)
#define LIGHT_INTENSITY_THRESHOLD 127

int read_ADC(void)
{
    // Select ADC channel for the light sensor
    ADC0->SC1[0] = ADC_SC1_ADCH(LIGHT_SENSOR_PIN);

    // Wait for the conversion to complete
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    // Read the ADC result
    return ADC0->R[0];
}

void init_ADC0(void)
{
    // Enable clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Configure ADC0
    ADC0->CFG1 = 0b10110000; // Set ADC to 16-bit single-ended, bus clock divided by 8, long sample time
    ADC0->CFG2 |= 0b10000;   // Set long sample time
    ADC0->SC3 |= 0b100;      // Set hardware average to 32 samples
}

void ADC_Calibration()
{
    uint16_t calib;

    // Start calibration process
    ADC0->SC3 |= ADC_SC3_CAL_MASK;

    // Wait for calibration to complete
    while (ADC0->SC3 & ADC_SC3_CAL_MASK)
        ;

    // Check for calibration failure
    //    if (ADC0->SC3 & ADC_SC3_CALF_MASK) {
    //        // Handle the error
    //        while (1);
    //    }

    // Calculate the calibration result
    calib = ADC0->CLP0 + ADC0->CLP1 + ADC0->CLP2 + ADC0->CLP3 + ADC0->CLP4 + ADC0->CLPS;
    calib = (calib >> 1) | 0x8000;
    ADC0->PG = calib;

    calib = ADC0->CLM0 + ADC0->CLMD;
    calib = ADC0->CLM0 + ADC0->CLM1 + ADC0->CLM2 + ADC0->CLM3 + ADC0->CLM4 + ADC0->CLMS;
    calib = (calib >> 1) | 0x8000;
    ADC0->MG = calib;
}

void short_delay()
{
	for(int i=1000000; i>0; i--){}
}


int read_ADC_avg(void)
{
    int sum = 0;
    for(int i=0; i<10; i++)
    {
        // Select ADC channel for the light sensor
        ADC0->SC1[0] = ADC_SC1_ADCH(LIGHT_SENSOR_PIN);

        // Wait for the conversion to complete
        while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

        // Read the ADC result
        sum += ADC0->R[0];
    }
    return sum / 10;
}

void SetupADC(){

	int cal_v;

	//Enable clock gate for ADC0
	SIM->SCGC6 |= (1 << 27);

	// Setup ADC
	ADC0->CFG1 = 0;  // Default everything.
	ADC0->CFG1 |= ADC_CFG1_ADICLK(0b00); // Use bus clock.
	ADC0->CFG1 |= ADC_CFG1_MODE(0b10);   // 00 for 8-bit
	                                     // 01 for 12-bit
	                                     // 10 for 10-bit
	                                     // 11 for 16-bit

	//Calibrate
	ADC0->SC3 = 0;
	ADC0->SC3 |= ADC_SC3_AVGS(0b11);  // SelectMaximum Hardware Averaging (32) see 28.3.7 for details
	ADC0->SC3 |= ADC_SC3_AVGE_MASK;   // Enable Hardware Averaging
	ADC0->SC3 |= ADC_SC3_CAL_MASK;    // Start Calibration

	// Wait for calibration to complete
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

	//Assume calibration worked, or check ADC_SC3_CALF

	// Calibration Complete, write calibration registers.
	cal_v = ADC0->CLP0 + ADC0->CLP1 + ADC0->CLP2 + ADC0->CLP3 + ADC0->CLP4 + ADC0->CLPS;
	cal_v = cal_v >> 1 | 0x8000;
	ADC0->PG = cal_v;

	cal_v = 0;
	cal_v = ADC0->CLM0 + ADC0->CLM1 + ADC0->CLM2 + ADC0->CLM3 + ADC0->CLM4 + ADC0->CLMS;
	cal_v = cal_v >> 1 | 0x8000;
	ADC0->MG = cal_v;


	ADC0->SC3 = 0;  //Turn off hardware averaging for faster conversion
			//or enable as above in calibration.
	return;
}

int main(void) {

	int light_reading;

    // printf initialization
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    SetupADC();
    LED_Initialize();

    SIM->SCGC5 |= (1<<13); // Enable Light Sensor I/O Port

    while(1) {

		ADC0->SC1[0] = ADC_SC1_ADCH(3); //Start conversion by writing the channel
										//to ADCH. The light sensor is on channel 3

		while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)); //Block until conversion is complete

		light_reading = ADC0->R[0];

		// display new reading
		PRINTF("New Reading: %d\n\r",light_reading);

		if (light_reading < 1000)
		{
			LEDGreen_On();
		}
		else
		{
			LED_Off();
		}
		short_delay();
    }
    return 0 ;
}
