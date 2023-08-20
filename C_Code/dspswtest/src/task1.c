/*
 * task1.c
 * Create a script to read in the testData.txt into your preferred language
 * Created on: Aug 19, 2023
 * Author: Sufiyan */

#include "dsp_task.h"

// Buffer to store the IQ samples from file
cmpx_double data[NUM_ANTENNAS][MAX_NUM_IQ_READ];

/**
 * @brief Reads complex IQ data from a file.
 * This function reads complex IQ data from a file named by the given filename.
 * @param filename The name of the file containing complex IQ data.
 * @return The number of samples read from the file.
 */
static int readComplexData(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error in opening file");
        exit(1);
    }
    int samp_idx = 0;
    while (!feof(file))
    {
        for (int i = 0; i < NUM_ANTENNAS; ++i)
        {
            if (fscanf(file, "%lf %lf", &data[i][samp_idx].real, &data[i][samp_idx].imag) != 2)
            {
            	printf("Ant%d Samp%d\n",i,samp_idx);
                perror("Error in reading IQ samples");
                fclose(file);
                exit(1);
            }
        }
        if(samp_idx > MAX_NUM_IQ_READ-1)
        {
        	printf("!!!Warning!!! Number of samples in File exceeds max allowed Buffer size\n");
        	break;
        }
        samp_idx++;
    }
    fclose(file);
    return samp_idx;
}

/**
 * @brief Prints complex IQ data .
 * This function prints the complex IQ data stored in the 'data' array.
 * @param numSamples The number of samples to be printed.
 */
static void printIQData(int numSamples)
{
	for (int samp_idx = 0; samp_idx < numSamples; ++samp_idx)
	{
		for (int ant_idx = 0; ant_idx < NUM_ANTENNAS; ++ant_idx)
		{
			printf("%2.6lf %2.6lf\t", data[ant_idx][samp_idx].real, data[ant_idx][samp_idx].imag);
			if (ant_idx == NUM_ANTENNAS - 1)
			{
				printf("\n");
			}
		}
	}
}

/**
 * @brief Reads complex IQ data from a file.
 * This function reads complex IQ data from a file named by the given filename and print it on console.
 * @param filename The name of the file containing complex IQ data.
 * @return The number of samples read from the file.
 */
int task1(const char *filename)
{
	int numSamplesRead = readComplexData(filename);
	printf("Succsesfully parsed %d samples\n", numSamplesRead);
    //printIQData(numSamplesRead); // Uncomment to print the IQ data
    return numSamplesRead;
}
