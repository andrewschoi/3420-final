#include <dataQueue.h>

int queue[QUEUE_SIZE];
int front = 0;
int rear = 0;

void enqueue(int data)
{
  // Check if queue is full
  if ((rear + 1) % QUEUE_SIZE == front)
  {
    // Queue is full, overwrite oldest element
    front = (front + 1) % QUEUE_SIZE;
  }

  // Insert new element
  queue[rear] = data;
  rear = (rear + 1) % QUEUE_SIZE;
}

int dequeue()
{
  // Check if queue is empty
  if (front == rear)
  {
    // Queue is empty, return error value
    return -1;
  }

  // Remove oldest element
  int data = queue[front];
  front = (front + 1) % QUEUE_SIZE;
  return data;
}
