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


void main(void)
{
    init_ADC0();
    ADC_Calibration();
    LED_Initialize();

    LED_Off();

    while (1)
    {
    	int light_reading = read_ADC_avg();
        if (light_reading > 20)
        {
			LEDGreen_On();
			delay();
//			LEDRed_Toggle();
        }
        else
        {
//			LEDGreen_Toggle();
//			LEDRed_On();
//			LEDGreen_Toggle();
//			green_on = 0;
        	LED_Off();
        	delay();

        }

    }
}
