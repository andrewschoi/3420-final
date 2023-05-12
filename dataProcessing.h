#ifndef __DATAPROCESSING_H__
#define __DATAPROCESSING_H__

// Returns whether the maximum difference of data points [input] of length [size]
// exceeds a pre-defined threshold [INTENSITY_DIFFERENCE_THRESHOLD] after gaussian
// filtering and normalization
int exceedDifferenceThreshold(int input[], int size);

#endif
