/*
 * task3.c
 *
 *  Created on: Aug 19, 2023
 *      Author: Sufiyan
 */


#include "dsp_task.h"

// Function to apply a moving average filter to an array
void movingAverageFilter(DataBuff* buffer, int ant, int windowSize)
{
	cmpx_double sum   = {.real = 0.0, .imag = 0.0};
	int start_idx     = buffer->out ;
	int rcvSampleSize = buffer->count;

    // Initialize the sum for the first window
    for (int i = start_idx; i < windowSize; i++)
    {
        sum.real += buffer->data[ant][i].real;
        sum.imag += buffer->data[ant][i].imag;
    }
    // Apply the moving average filter
    for (int i = windowSize; i < rcvSampleSize; i++)
    {
        // Calculate the average for the current window
    	buffer->data[ant][i - windowSize].real = sum.real / windowSize;
    	buffer->data[ant][i - windowSize].imag = sum.imag / windowSize;

        // Update the sum for the next window
        sum.real = sum.real - buffer->data[ant][i - windowSize].real + buffer->data[ant][i].real;
        sum.imag = sum.imag - buffer->data[ant][i - windowSize].imag + buffer->data[ant][i].imag;
    }

    // Calculate the average for the last window
    buffer->data[ant][rcvSampleSize - windowSize].real = sum.real / windowSize;
    buffer->data[ant][rcvSampleSize - windowSize].imag = sum.imag / windowSize;

    // Fill the remaining elements with the last calculated average
    for (int i = rcvSampleSize - windowSize; i < rcvSampleSize; i++) {
    	buffer->data[ant][i].real = buffer->data[ant][rcvSampleSize - windowSize].real;
    	buffer->data[ant][i].imag = buffer->data[ant][rcvSampleSize - windowSize].imag;
    }
}
