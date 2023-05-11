#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#define QUEUE_SIZE 20

// Queues [data] onto queue
void enqueue(int data);

// Pops off [data] from queue
int dequeue(void);

// Returns whether the sampled data exceeds [LIGHT_INTENSITY_THRESHOLD]
int doesExceedThreshold(void);

#endif
