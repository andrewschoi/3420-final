#include <stdio.h>
#include <math.h>
#include <dataProcessing.h>

#define PI 3.14159265
#define SIGMA 1.0
#define FILTER_SIZE 5
#define INTENSITY_DIFFERENCE_THRESHOLD 1

// Modifies [filter] to be a 1D Gaussian Filter
void calculateGaussianFilter(double filter[])
{
  double sum = 0.0;
  int start = -FILTER_SIZE / 2;
  int end = FILTER_SIZE / 2;
  for (int x = start; x <= end; x++)
  {
    filter[x - start] = (1 / sqrt(2 * PI * pow(SIGMA, 2))) * exp(-pow(x, 2) / (2 * pow(SIGMA, 2)));
    sum += filter[x - start];
  }

  // Normalize the filter
  for (int i = 0; i < FILTER_SIZE; i++)
    filter[i] /= sum;
}

// Modifies input array [input] of size [size] by applying [filter] on [input]
void applyGaussianFilter(double input[], double filter[], int size)
{
  double sum = 0.0;

  // Apply the Gaussian filter
  for (int i = FILTER_SIZE / 2; i < size - FILTER_SIZE / 2; i++)
  {
    sum = 0.0;

    for (int j = 0; j < FILTER_SIZE; j++)
      sum += input[i + j - FILTER_SIZE / 2] * filter[j];

    input[i] = sum;
  }

  // Find the maximum and minimum values in the filtered data
  double min_val = min(input, size);
  double max_val = max(input, size);

  // Normalize the filtered data
  for (int i = 0; i < size; i++)
    input[i] = (input[i] - min_val) / (max_val - min_val);
}

// Gives whether the maximum difference in [input] (after gaussian filtering)
// exceeds the [INTENSITY_DIFFERENCE_THRESHOLD]
int exceedDifferenceThreshold(double input[], int size)
{
  double filter[FILTER_SIZE];
  calculateGaussianFilter(filter);

  double inputCopy[size];
  for (int i = 0; i < size; i++)
  {
    inputCopy[i] = input[i];
  }

  applyGaussianFilter(inputCopy, filter, size);

  if (max(inputCopy, size) - min(inputCopy, size) > INTENSITY_DIFFERENCE_THRESHOLD)
    return 1;
  return 0;
}

// Gives maximum of an input array of size [size]
double max(double input[], int size)
{
  if (size <= 0)
  {
    return 0;
  }

  double maximum = input[0];
  for (int i = 1; i < size; i++)
  {
    if (input[i] > maximum)
    {
      maximum = input[i];
    }
  }
  return maximum;
}

// Gives minimum of an input array of size [size]
double min(double input[], int size)
{
  if (size <= 0)
  {
    return 0;
  }

  double minimum = input[0];
  for (int i = 1; i < size; i++)
  {
    if (input[i] < minimum)
    {
      minimum = input[i];
    }
  }
  return minimum;
}