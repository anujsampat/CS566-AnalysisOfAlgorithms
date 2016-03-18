/*
 * Anuj Sampat
 * asampat@bu.edu
 *
 * Implementation of Divide And Conquer Method for Max SubArray Problem.
 */

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

//#define DEBUG

#define MAX_INPUT_SIZE	20000


static void find_max_crossing_subarray (long int A[], int low, int mid, int high,
			    int *cross_low, int *cross_high,
			    long int *cross_sum)
{
  long int sum = 0;
  long int left_sum = LONG_MIN;
  long int right_sum = LONG_MIN;
  int i;

  for (i = mid; i >= low; i--)
    {
      sum += A[i];
      if (sum > left_sum)
	{
	  left_sum = sum;
	  *cross_low = i;
	}
    }

  sum = 0;
  for (i = mid + 1; i <= high; i++)
    {
      sum += A[i];
      if (sum > right_sum)
	{
	  right_sum = sum;
	  *cross_high = i;
	}
    }

  *cross_sum = left_sum + right_sum;

}

static void divide_conquer_max_subarray (long int A[], int low, int high, int *index_low,
			     int *index_high, long int *sum)
{
  int left_low, left_high;
  int right_low, right_high;
  int cross_low, cross_high;
  long int left_sum, right_sum, cross_sum;
  int mid;

  if (low == high)
    {
      *index_high = high;
      *index_low = low;
      *sum = A[low];
    }
  else
    {
      mid = (low + high) / 2;

      divide_conquer_max_subarray (A, low, mid, &left_low, &left_high,
				   &left_sum);
      divide_conquer_max_subarray (A, mid + 1, high, &right_low, &right_high,
				   &right_sum);
      find_max_crossing_subarray (A, low, mid, high, &cross_low, &cross_high,
				  &cross_sum);

      if (left_sum >= right_sum && left_sum >= cross_sum)
	{
	  *sum = left_sum;
	  *index_low = left_low;
	  *index_high = left_high;
	}
      else if (right_sum >= left_sum && right_sum >= cross_sum)
	{
	  *sum = right_sum;
	  *index_low = right_low;
	  *index_high = right_high;
	}
      else
	{
	  *sum = cross_sum;
	  *index_low = cross_low;
	  *index_high = cross_high;
	}
    }
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

  divide_conquer_max_subarray (inputArray, 0, inputSize - 1, &low, &high,
			       &max);

  gettimeofday (&endTime, NULL);

  elapsedTimeMS =
    (((endTime.tv_sec - startTime.tv_sec) * 1000000) +
     (endTime.tv_usec - startTime.tv_usec)) / 1000;

  printf ("n = %d, elapsed time in ms = %d, low = %d, high = %d, sum = %ld\n",
	  inputSize, elapsedTimeMS, low, high, max);

  return 0;
}
