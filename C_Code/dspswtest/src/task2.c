/*
 * task2.c
 * Estimate DC offset  for all elements. Consider the recorded data in the text file as input
 * from a real-time system. Designing a buffer to emulate real-time input shall be considered.
 * Implement DC offset algorithm in language selected in Task 1.Utilizing toolbox and array is
 * strongly recommended.
 *
 * Created on: Aug 19, 2023
 * Author: Sufiyan
 */

#include "dsp_task.h"


DataBuff buffer;
DcOffset dcOffsets[NUM_ANTENNAS]; // One DC offset estimate for each antenna
int window_count=0;

/*
 * @brief Initialize the circular buffer and DC offset structure.
 * @param 'buffer'  pointer to the circular buffer.
 * @param 'dcOffsets' An array of DcOffset structures for each antenna.
 */
static void init(DataBuff *buffer,  DcOffset *dcOffsets)
{
	buffer->count = 0;
	buffer->in = 0;
	buffer->out = 0;
	for (int i = 0; i < NUM_ANTENNAS; ++i)
	{
		dcOffsets[i].DcOffset = 0.0;
		dcOffsets[i].averageDcOffsetAnt = 0.0;
		dcOffsets[i].count = 0;
	}
}

/**
 * @brief Read complex data from the file in chunks of BUFFER_SIZE and store it in the circular buffer.
 * @param 'buffer' pointer to the circular buffer.
 * @param 'file'   pointer to the input file.
 */
void readData(DataBuff *buffer, FILE *file)
{
	int valuesRemain = 1;
	while (buffer->count < BUFFER_SIZE)
	{
		for (int ant = 0; ant < NUM_ANTENNAS; ++ant)
		{
			if(fscanf(file, "%lf %lf", &buffer->data[ant][buffer->in].real, &buffer->data[ant][buffer->in].imag)!=2)
			{
				valuesRemain=0;
				break;
			}
		}
		if(valuesRemain==0)
			break;
		buffer->in = (buffer->in + 1) % BUFFER_SIZE;
		buffer->count++;
	}
}

/*
 * @brief Estimate DC offset for each antenna.
 * @param 'buffer'  pointer to the circular buffer.
 * @param 'dcOffsets' An array of DcOffset structures for each antenna.
 */
void estimateDcOffset(DataBuff* buffer, DcOffset *dcOffsets)
{
	int size = buffer->count;
	cmpx_double sumDcOffset[NUM_ANTENNAS] = {{0.0, 0.0}}; // Initialize to zero
	while (buffer->count > 0)
	{
		for (int ant = 0; ant < NUM_ANTENNAS; ++ant)
		{
			sumDcOffset[ant].real += buffer->data[ant][buffer->out].real;
			sumDcOffset[ant].imag += buffer->data[ant][buffer->out].imag;

			//printf("Ant[%d] data = %lf %lf\n",ant, buffer->data[ant][buffer->out].real,  buffer->data[ant][buffer->out].imag);
		}
		buffer->out = (buffer->out + 1) % BUFFER_SIZE;
		buffer->count--;
	}
	// Calculate DC offset and store in dcOffsets
	window_count++;
	for (int ant = 0; ant < NUM_ANTENNAS; ++ant) {
		double scaleFactor = 1.0 / size;
		dcOffsets[ant].DcOffset = cabs((sumDcOffset[ant].real + sumDcOffset[ant].imag*I) * scaleFactor);
		dcOffsets[ant].averageDcOffsetAnt += dcOffsets[ant].DcOffset;  // used in reporting estimated values
		dcOffsets[ant].count = window_count;
		//printf("Antenna %d DC Offset: %lf  window %d\n", ant, dcOffsets[ant].DcOffset, window_count);
	}
}

/*
 * @brief task2 function.
 * Reads complex data from a file, estimates DC offset for each antenna,
 * calculates average DC offset, and prints the results.
 * @return Returns 0 on successful execution, non-zero on error.
 */
int task2(const char *filename,bool isFilteredSignal)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening the file");
		return 1;
	}

	// Moving buffers to global as it can cause stack overflow issue for larger buffer size
	//DataBuff buffer;
	//DcOffset dcOffsets[NUM_ANTENNAS]; // One DC offset estimate for each antenna
	init(&buffer, dcOffsets);

	while (!feof(file))
	{
		readData(&buffer, file);
		if (buffer.count == BUFFER_SIZE)
		{
			if(isFilteredSignal)
				for (int ant = 0; ant < NUM_ANTENNAS; ++ant)
					movingAverageFilter(&buffer, ant, AVERAGING_WINDOW);

			estimateDcOffset(&buffer,dcOffsets);
		}
	}

	// Process remaining values in the buffer
	if (buffer.count > 0)
	{
		buffer.count = BUFFER_SIZE;
		if(isFilteredSignal)
			for (int ant = 0; ant < NUM_ANTENNAS; ++ant)
				movingAverageFilter(&buffer, ant, AVERAGING_WINDOW);

		estimateDcOffset(&buffer,dcOffsets);
	}

	printf("********************** Averaged DC Offset Estimation for all window ********************** \n");
	for (int ant = 0; ant < NUM_ANTENNAS; ++ant){
		dcOffsets[ant].averageDcOffsetAnt = dcOffsets[ant].averageDcOffsetAnt/window_count;
		printf("Antenna %d DC Offset: %lf\n", ant, dcOffsets[ant].averageDcOffsetAnt);
	}
	fclose(file);

	return 0;
}
