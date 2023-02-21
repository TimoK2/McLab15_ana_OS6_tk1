/* Description: McLab15_SensorAna_OS6_tk1
 * Hardware: L432KC or similar.
 * For testing a microwave "radar" sensor 
 * MIKROE Microwave Click or similar. 
 * Connect:
 * L432KC A6 -- sensor output (Out)
 * GND rail  -- senror GND (GND)
 * 3.3V rail -- sensor Power (5V)
 * 
 * L432KC LD3 is permanently connected to D13
 * and can be named LED1 in the code.
 * Operating system OS 6.0...6.16
 * Timo Karppinen 8.12.2022  Apache-2.0
 */

#include "mbed.h"
// Sampling rate n milliseconds
#define SAMPLING_RATE     2ms
// Output change every n seconds
#define OUTPUT_UPDATE     1s

float mwSampleMean = 0.0;  // from 0.0 to 1.0
int mwSampleMeanInt = 0;   // from 0 to 100

// An other thread for timing the sensor signal sampling
Thread thread2; 

void sample(){
    // Initialise the analog pin A5 as an analog input
   
    AnalogIn mwSen(A6);
    // Variables for the microwave signal samples
    float iSample;
    float aSample;
    static float mwSamples[100];   // from 0.0 to 1.0 
    static float mwSampleSum = 0;  // better initialize with 0
    static int index =0;
    
    while (true) {
        iSample = mwSen.read();
        aSample = iSample;
        //for inverting peaks when peaks are positive. Zero level is 0.5 
        // Remove the next if statement if inverting peaks is not needed. 
        if(aSample < 0.5){
            aSample = 1 - aSample;
        }
        mwSamples[index] = aSample;
        // Printing for first tests. Sampling 100 ms, update 10 s
        // Comment the print line. Change sample and out update periods
        // into about a few millisseconds and one second. 
        // printf("iSample%d mwSample%d at%d\n",(int)(100*iSample), (int)(100*mwSamples[index]), index);
        if(index < 99){
            mwSampleSum = mwSampleSum + mwSamples[index] - mwSamples[index+1];
        }
        if(index >= 99){
            mwSampleSum = mwSampleSum + mwSamples[99] - mwSamples[0];
            index = -1;
        }
        index = index +1;
        mwSampleMean = mwSampleSum/100;
        ThisThread::sleep_for(SAMPLING_RATE);
    }
}

int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut ledA(LED1);
    int ledAState = false;

    thread2.start(callback(sample));

    while (true) {
        ledA.write(ledAState);
        mwSampleMeanInt = (int)(100*mwSampleMean);
        if(mwSampleMeanInt > 70){
            ledAState = true;
        }  else{
            ledAState = false; 
        }
        ledA.write(ledAState);
        printf("MW Sensor mean reading  %d Detected %d\n", mwSampleMeanInt, ledAState );
        ThisThread::sleep_for(OUTPUT_UPDATE);
    }
}

