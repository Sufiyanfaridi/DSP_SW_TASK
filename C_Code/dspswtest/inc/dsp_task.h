/*
 * dsp_task.h
 *
 *  Created on: Aug 19, 2023
 *      Author: Sufiyan
 */

#ifndef INC_DSP_TASK_H_
#define INC_DSP_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NUM_IQ_READ  10000 // get it from cmd line
#define BUFFER_SIZE      400    // Buffer size window to calculate DC offset
#define NUM_ANTENNAS     8      // Maximum Number of Rcv Antenna Signal
#define Fs               4000
#define AVERAGING_WINDOW 50    // Adjust the window size as needed

// Complex IQ data with I & Q stores double value.
typedef struct {
	double real;
	double imag;
} cmpx_double;

// Report DC Offset Estimation.
typedef struct {
	int count;                  // Counter to track sum of DC Offset used while averaging.
	double DcOffset;            // DC Offset estimation value .
	double averageDcOffsetAnt;  // DC Offset estimate average over all antennas
} DcOffset;

// Circular Buffer structure
typedef struct {
	int in;         			// to track data read index for filling 'data' buffer
	int out;        			// to track consume index for fetching content of 'data' buffer
	int count;      			// keep track the read/write size of buffer
	cmpx_double data[NUM_ANTENNAS][BUFFER_SIZE];   // Holds complex IQ samples
} DataBuff;

void movingAverageFilter(DataBuff* buffer, int ant, int windowSize);
int task1(const char *filename);
int task2(const char *filename, bool isFilteredSignal);
int task3(const char *filename);

#endif /* INC_DSP_TASK_H_ */
