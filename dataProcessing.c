#include <stdio.h>
#include <math.h>
#include <dataProcessing.h>

#define PI 3.14159265
#define SIGMA 1.0
#define FILTER_SIZE 5
#define INTENSITY_DIFFERENCE_THRESHOLD 0.1

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
      int index = (i + j - FILTER_SIZE / 2 + size) % size; // wrap around
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

int exceedDifferenceThreshold(double input[], int size)
{
  double filter[FILTER_SIZE];
  calculateGaussianFilter(filter);

  double *inputCopy = (double *)malloc(size * sizeof(double));
  if (inputCopy == NULL)
  {
    // Handle error
  }

  for (int i = 0; i < size; i++)
  {
    inputCopy[i] = input[i];
  }

  double *result = (double *)malloc(size * sizeof(double));
  if (result == NULL)
  {
    // Handle error
  }

  applyGaussianFilter(inputCopy, filter, size, result);

  if (max(result, size) - min(result, size) > INTENSITY_DIFFERENCE_THRESHOLD)
    return 1;

  if (max(result, size) - min(result, size) > INTENSITY_DIFFERENCE_THRESHOLD)
  {
    free(inputCopy);
    free(result);
    return 1;
  }
  free(inputCopy);
  free(result);
  return 0;
}