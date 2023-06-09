#include <stdio.h>
#include <math.h>
#include <dataProcessing.h>

#include <stdio.h>
#include <pin_mux.h>
#include <clock_config.h>
#include <board.h>
#include <fsl_debug_console.h>
#include <MKL46Z4.h>

#define PI 3.14159265
#define SIGMA 1.0
#define FILTER_SIZE 6
#define INTENSITY_DIFFERENCE_THRESHOLD 1000

double max(double input[], int size)
{
  if (size <= 0)
    return 0;

  double maximum = input[0];
  for (int i = 1; i < size; i++)
    if (input[i] > maximum)
      maximum = input[i];

  return maximum;
}

double min(double input[], int size)
{
  if (size <= 0)
    return 0;

  double minimum = input[0];
  for (int i = 1; i < size; i++)
    if (input[i] < minimum)
      minimum = input[i];

  return minimum;
}

void calculateGaussianFilter(double filter[])
{
  double sum = 0.0;
  int start = -FILTER_SIZE / 2;
  int end = FILTER_SIZE / 2;

  for (int x = start; x <= end; x++)
  {
    filter[x - start] = (1.0 / sqrt(2 * PI * pow(SIGMA, 2))) * exp(-pow(x, 2) / (2 * pow(SIGMA, 2)));
    sum += filter[x - start];
  }

  for (int i = 0; i < FILTER_SIZE; i++)
    filter[i] /= sum;
}

void applyGaussianFilter(double input[], double filter[], int size, double result[])
{
  for (int i = 0; i < size; i++)
  {
    double sum = 0.0;
    for (int j = 0; j < FILTER_SIZE; j++)
    {
      int index = (int)(i + j - FILTER_SIZE / 2 + size) % size; // wrap around
      sum += input[index] * filter[j];
    }
    result[i] = sum;
  }

  // Find the maximum and minimum values in the filtered data
  double min_val = min(result, size);
  double max_val = max(result, size);

  // Normalize the filtered data
  for (int i = 0; i < size; i++)
    result[i] = (result[i] - min_val) / (max_val - min_val);
}

int exceedDifferenceThreshold(int input[], int size)
{
  double filter[FILTER_SIZE];
  calculateGaussianFilter(filter);

  double result[size];

  applyGaussianFilter(input, filter, size, result);
  double difference = max(result, size) - min(result, size);
  // PRINTF("difference %d ", difference);
  int last = input[size - 1];

  if (last > INTENSITY_DIFFERENCE_THRESHOLD)
  {
    return 1;
  }
  return 0;
}
