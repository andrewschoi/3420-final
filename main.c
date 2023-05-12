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

#define LIGHT_SENSOR_PIN 22
#define LIGHT_INTENSITY_THRESHOLD 1000
#define DATA_INGESTION_PERIOD 30

void SetupADC()
{

    int cal_v;

    // Enable clock gate for ADC0
    SIM->SCGC6 |= (1 << 27);

    // Setup ADC
    ADC0->CFG1 = 0;                      // Default everything.
    ADC0->CFG1 |= ADC_CFG1_ADICLK(0b00); // Use bus clock.
    ADC0->CFG1 |= ADC_CFG1_MODE(0b10);   // 00 for 8-bit
                                         // 01 for 12-bit
                                         // 10 for 10-bit
                                         // 11 for 16-bit

    // Calibrate
    ADC0->SC3 = 0;
    ADC0->SC3 |= ADC_SC3_AVGS(0b11); // SelectMaximum Hardware Averaging (32) see 28.3.7 for details
    ADC0->SC3 |= ADC_SC3_AVGE_MASK;  // Enable Hardware Averaging
    ADC0->SC3 |= ADC_SC3_CAL_MASK;   // Start Calibration

    // Wait for calibration to complete
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
        ;

    // Assume calibration worked, or check ADC_SC3_CALF

    // Calibration Complete, write calibration registers.
    cal_v = ADC0->CLP0 + ADC0->CLP1 + ADC0->CLP2 + ADC0->CLP3 + ADC0->CLP4 + ADC0->CLPS;
    cal_v = cal_v >> 1 | 0x8000;
    ADC0->PG = cal_v;

    cal_v = 0;
    cal_v = ADC0->CLM0 + ADC0->CLM1 + ADC0->CLM2 + ADC0->CLM3 + ADC0->CLM4 + ADC0->CLMS;
    cal_v = cal_v >> 1 | 0x8000;
    ADC0->MG = cal_v;

    ADC0->SC3 = 0; // Turn off hardware averaging for faster conversion
                   // or enable as above in calibration.
    return;
}

int main(void)
{

    int samplePoint;
    int isExceedingThreshold;

    // printf initialization
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    SetupADC();
    LED_Initialize();

    SIM->SCGC5 |= (1 << 13); // Enable Light Sensor I/O Port

    // Fill up the data queue
    for (int i = 0; i < DATA_INGESTION_PERIOD; i++)
    {
        ADC0->SC1[0] = ADC_SC1_ADCH(3); // Start conversion by writing the channel
                                        // to ADCH. The light sensor is on channel 3

        while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
            ; // Block until conversion is complete

        samplePoint = ADC0->R[0];
        enqueue(samplePoint);
    }

    // Poll light sensor
    while (1)
    {

        ADC0->SC1[0] = ADC_SC1_ADCH(3); // Start conversion by writing the channel
                                        // to ADCH. The light sensor is on channel 3

        while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
            ; // Block until conversion is complete

        samplePoint = ADC0->R[0];

        // display new reading
        PRINTF("New Reading: %d\n\r", samplePoint);

        enqueue(samplePoint);                         // queues the sampled data
        isExceedingThreshold = doesExceedThreshold(); // gives whether we exceed the light intensity threshold
        PRINTF("threshold: %d\n", isExceedingThreshold);

        if (isExceedingThreshold)
        {
            LEDGreen_On();
        }
        else
        {
            LED_Off();
        }
    }
    return 0;
}
