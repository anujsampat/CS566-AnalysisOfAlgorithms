/*
 * Anuj Sampat
 * asampat@bu.edu
 *
 * Implementation of the Brute Force Method for the Max SubArray Problem.
 */

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

//#define DEBUG

#define MAX_INPUT_SIZE	20000


static void brute_force_max_subarray (long int A[], int size, int *low, int *high,
			  long int *max)
{
  int l = 0, h = 0;
  long int currentMax = LONG_MIN;

  for (l = 0; l < size; l++)
    {
      *max = 0;

      for (h = l; h < size; h++)
	{
	  *max = *max + A[h];
	  if (*max > currentMax)
	    {
	      currentMax = *max;
	      *low = l;
	      *high = h;
	    }
	}
    }

  *max = currentMax;
}

int main (int argc, char *argv[])
{
  FILE *inputFile = NULL;
  long int inputArray[MAX_INPUT_SIZE];
  long int currentVal;
  int inputSize = 0;
  int i = 0;
  int low, high;
  long int max;

  long int elapsedTimeMS;	// elapsed time in milliseconds
  struct timeval startTime;
  struct timeval endTime;

  if (argv[1] == NULL)
    {
      printf ("Please provide input file path\n");
      printf ("BruteForce <input file path>\n");
      return -1;
    }

#ifdef DEBUG
  printf ("Input file path = %s\n", argv[1]);
#endif

  inputFile = fopen (argv[1], "r");
  if (inputFile == NULL)
    {
      printf ("Could not find input file\n");
      return -1;
    }
  else
    {
      while (fscanf (inputFile, "%ld", &currentVal) != EOF)
	{
	  if (inputSize == MAX_INPUT_SIZE)
	    {
	      printf ("Maximum input size exceeded, exiting\n");
	      return -1;
	    }

	  inputArray[inputSize] = currentVal;
	  inputSize++;
	}

      fclose (inputFile);
    }

  if (inputSize == 0)
    {
      printf ("No input data, exiting\n");
      return -1;
    }

  gettimeofday (&startTime, NULL);

  brute_force_max_subarray (inputArray, inputSize, &low, &high, &max);

  gettimeofday (&endTime, NULL);

  elapsedTimeMS = (((endTime.tv_sec - startTime.tv_sec) * 1000000) +
                    (endTime.tv_usec - startTime.tv_usec)) / 1000;

  printf ("n = %d, elapsed time in ms = %d, low = %d, high = %d, sum = %ld\n",
	  inputSize, elapsedTimeMS, low, high, max);

  return 0;
}
