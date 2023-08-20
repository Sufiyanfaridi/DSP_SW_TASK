/*
 * test.c
 *
 *  Created on: Aug 20, 2023
 *      Author: Lenovo
 */

#include "dsp_task.h"

#define NUM_SAMPLES       MAX_NUM_IQ_READ             // number of time domain samples to generate
#define MAX_ERR_VAR        5                          // Maximum error percentage(adjust accordingly) allowed to report test pass or fail.

cmpx_double refdcOffset[NUM_ANTENNAS];                // Holds reference DC offset value for each antenna
cmpx_double whiteNoise[NUM_ANTENNAS][NUM_SAMPLES];    //Holds complex white Gaussian noise for each antenna
cmpx_double receivedSignal[NUM_ANTENNAS][NUM_SAMPLES];//Holds complex IQ data for each antenna

extern  DcOffset dcOffsets[NUM_ANTENNAS];              // Estimated DC offset for each antenna from task2.c

double snrdB,  samplingFreq ,signalFrequency ,dcOffsetReal,dcOffsetImag;

void writeDataToFile(const char *filename, cmpx_double receivedSignal[][NUM_SAMPLES])
{
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		perror("Error opening file");
		exit(1);
	}

	for (int sample = 0; sample < NUM_SAMPLES; sample++) {
		for (int antenna = 0; antenna < NUM_ANTENNAS; antenna++) {
			fprintf(file, "%lf %lf", receivedSignal[antenna][sample].real, receivedSignal[antenna][sample].imag);
			if (antenna < NUM_ANTENNAS - 1) {
				fprintf(file, " ");
			}
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// Function to generate complex white Gaussian noise
void generateNoise(cmpx_double *noise, int numSamples, double snr_dB)
{
	double noiseVar = 1.0 / pow(10.0, snr_dB / 10.0); // Noise variance

// sqrt(noiseVar / 2.0) scales the noise to achieve the desired variance while ensuring that
// the noise has a Gaussian distribution.
//((double)rand() / RAND_MAX * 2.0 - 1.0) generates a random number between -1 and 1, which
// contributes to the real and imaginary parts of the complex noise sample.

	for (int i = 0; i < numSamples; i++)
	{
		double noiseReal = sqrt(noiseVar / 2.0) * ((double)rand() / RAND_MAX * 2.0 - 1.0);
		double noiseImag = sqrt(noiseVar / 2.0) * ((double)rand() / RAND_MAX * 2.0 - 1.0);
		noise[i].real = noiseReal;
		noise[i].imag = noiseImag;
	}
}

int createWaveform(const char *filename)
{
	// Seed the random number generator
	srand(time(NULL));
/*	// Set parameters for different scenarios, it will Set up test vector data
	double snrdB           = 15.0 ; // Signal-to-Noise Ratio in dB
	double samplingFreq    = Fs;     // Sampling Frequency in Hz
	double signalFrequency = 400.0;  // Frequency of the sinusoidal signal in Hz (unknown)
	double dcOffsetReal    = 0.25;   // Real part of DC offset
	double dcOffsetImag    = 0.010;  // Imaginary part of DC offset
*/
	// Generate complex white Gaussian noise for each antenna
	for (int antenna = 0; antenna < NUM_ANTENNAS; antenna++)
	{
		generateNoise(whiteNoise[antenna], NUM_SAMPLES, snrdB);
	}

	// Generate the received signal for each antenna based on recieve model  𝑦(m,𝑛) = 𝑒^(j*2*pi*f*n/fs) + 𝐷(m) + 𝑃(m,𝑛)
	for (int antenna = 0; antenna < NUM_ANTENNAS; antenna++)
	{
		refdcOffset[antenna].real = dcOffsetReal;
		refdcOffset[antenna].imag = dcOffsetImag;
		for (int sample = 0; sample < NUM_SAMPLES; sample++)
		{
			double time = (double)sample / samplingFreq;
			double signalValueReal = cos(2.0 * M_PI * signalFrequency * time);
			double signalValueImag = sin(2.0 * M_PI * signalFrequency * time);
			receivedSignal[antenna][sample].real = signalValueReal + dcOffsetReal + whiteNoise[antenna][sample].real;
			receivedSignal[antenna][sample].imag = signalValueImag + dcOffsetImag + whiteNoise[antenna][sample].imag;

		}
	}
	// Write the data to a file
	writeDataToFile(filename, receivedSignal);
	return 0;
}

// Define test data
// You should have different data sets for different test scenarios

void testTask2DcOffsetEst(const char *filename)
{
	task2(filename,0);
	printf("\n**********************************  Validating test   **********************************\n");
	for (int antenna = 0; antenna < NUM_ANTENNAS; ++antenna)
	{
		double estErr = 100 * fabs(dcOffsets[antenna].averageDcOffsetAnt-refdcOffset[antenna].real) / refdcOffset[antenna].real;
		// Verify the results
		if (estErr > MAX_ERR_VAR)
		{
			printf("Ant[%d] Test Failed: Incorrect Estimated DC ofsset : ",antenna);
			printf("Est Error %lf%%, max allowed err %d%%\n", estErr, MAX_ERR_VAR );
		}
		else if (estErr == 0)
		{
			printf("Ant[%d] Test Passed: Estimated DC ofsset exactly matches with reference\n",antenna);
		}
		else
		{
			printf("Ant[%d] Test Passed: Estimated DC ofsset almost identical with reference : ",antenna);
			printf("Est Error %lf%%, max allowed err %d%%\n", estErr, MAX_ERR_VAR );
		}
	}
}

int main(int argc, char *argv[])
{
	printf("\n************************************  Command Usage   ************************************\n");
	printf(" arguments:  <snr=10> <fs=4000> <f=400> <dc=.25>\n");
	printf(" ex: ./unitTest snr=10 fs=4000 f=400 dc=.25\n");

	//Default Parameters to test
	const char *filename = "testVector.txt";
	snrdB           	 = 15.0 ; // Signal-to-Noise Ratio in dB
	samplingFreq    	 = Fs;     // Sampling Frequency in Hz
	signalFrequency 	 = 400.0;  // Frequency of the sinusoidal signal in Hz (unknown)
	dcOffsetReal    	 = 0.25;   // Real part of DC offset
	dcOffsetImag    	 = 0.0;  // Imaginary part of DC offset
	char argv_in[100];

	// Set parameters for different scenarios from command line, it will Set up test vector data
	for (int i=1; i< argc; i++)
	{
		strcpy(argv_in, argv[i]);
		if(strstr(argv[i], "snr") != NULL)
		{
			strtok(argv_in, "=");
			snrdB  = strtod(strtok(NULL, "="),NULL);
		}
		else if (strstr(argv[i], "fs") != NULL)
		{
			strtok(argv_in, "=");
			samplingFreq= strtod(strtok(NULL, "="),NULL);
		}
		else if (strstr(argv[i], "f") != NULL)
		{
			strtok(argv_in, "=");
			signalFrequency = strtod(strtok(NULL, "="),NULL);
		}
		else if (strstr(argv[i], "dc") != NULL)
		{
			strtok(argv_in, "=");
			dcOffsetReal = strtod(strtok(NULL, "="),NULL);
		}
		memset(argv_in, 0, 100);
	}

	printf("\n**********************************  Running Unit test   **********************************\n");
	printf("snrdB[%.2lf] samplingFreq[%.2lf] signalFrequency[%.2lf] dcOffset[%.2lf]\n", snrdB, samplingFreq, signalFrequency, dcOffsetReal);

	createWaveform(filename);
	testTask2DcOffsetEst(filename);

	return 0;
}
