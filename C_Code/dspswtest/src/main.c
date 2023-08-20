/*
 * main.c
 *
 *  Created on: Aug 19, 2023
 *      Author: Sufiyan
 */

#include "dsp_task.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Invalid Run Command Usage: ./dspswtest <task>\n");
		printf("Available tasks:\n");
		printf("task1: Read the testData.txt file in Buffer.command to execute on CLI: ./make_test_try.exe task1 \n");
		printf("task2: Implementation DC offset algorithm.  command to execute on CLI: ./make_test_try.exe task2 \n");
		printf("task3: Improved DC Offset Estimation algo.  command to execute on CLI: ./make_test_try.exe task3 \n");
		return 1;
	}

    //File testDataOriginal.txt contain recordings from 8 antenna elements in complex IQ samples at 4000Hz.
	const char *filename = "testDataOriginal.txt";
	if (strcmp(argv[1], "task1") == 0)
	{
		task1(filename);    //  Read samples from .txt file
	}
	else if (strcmp(argv[1], "task2") == 0)
	{
		task2(filename,0);  //  DC Offset Estimation
	}
	else if (strcmp(argv[1], "task3") == 0)
	{
		task2(filename,1);  // Improved DC Offset Estimation with Noise Averaging
	}
	else
	{
		printf("Invalid task name: %s\n", argv[1]);
		return 1;
	}

    return 0;
}

