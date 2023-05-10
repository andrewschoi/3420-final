#include <MKL46Z4.h>
#include <utils.h>
#include <stdint.h>
#include <dataProcessing.h>
#include <dataQueue.h>

#define LIGHT_SENSOR_PIN (20)
#define LIGHT_INTENSITY_THRESHOLD 127

int main(void)
{
    init_ADC0();
    ADC_Calibration();
    LED_Initialize();

    int green_on = 0;
    LEDRed_On();

    while (1)
    {
        if (read_ADC() > LIGHT_INTENSITY_THRESHOLD)
        {
            if (green_on == 0)
            {
                LEDRed_Toggle();
                LEDGreen_Toggle();
                delay();
                delay();
                delay();
                green_on = 1;
            }
        }
        else
        {
            if (green_on == 1)
            {
                LEDGreen_Toggle();
                LEDRed_Toggle();
                delay();
                green_on = 0;
            }
        }
        delay();
    }
}

int read_ADC(void)
{
    // Select ADC channel for the light sensor
    ADC0->SC1[0] = ADC_SC1_ADCH(LIGHT_SENSOR_PIN);

    // Wait for the conversion to complete
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
        ;

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
