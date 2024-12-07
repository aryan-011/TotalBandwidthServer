#ifndef STRUCTURES_H
#define STRUCTURES_H

struct taskName
{
	char taskType;
	int taskId;
};

struct periodicTask
{
	struct taskName name;
	int executionTime;
	int period;
};

struct aperiodicTask
{
	struct taskName name;
	int arrivalTime;
	int executionTime;
};

struct taskQueue
{
	struct taskName name;
	int arrivalTime;
	int executionTime;
	int deadline;
};

#endif