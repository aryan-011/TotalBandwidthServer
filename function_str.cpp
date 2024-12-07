#include <bits/stdc++.h>
#include "Structures.h"

using namespace std;

bool comparePeriod(struct periodicTask a, struct periodicTask b) { return a.period < b.period; }
bool compareArrival(struct aperiodicTask a, struct aperiodicTask b) { return a.arrivalTime < b.arrivalTime; }
bool compareQueueArrival(struct taskQueue a, struct taskQueue b) { return a.arrivalTime < b.arrivalTime; }
bool compareQueueDeadline(struct taskQueue a, struct taskQueue b) { return a.deadline < b.deadline; }

void removeFirstTask(vector<struct taskQueue> *tasks) { tasks->erase(tasks->begin()); }

double calculateUtilization(int count, struct periodicTask tasks[])
{
    double utilization = 0.0;
    for (int i = 0; i < count; i++)
        utilization += tasks[i].executionTime / double(tasks[i].period);
    return utilization;
}

void addDeadlines(vector<struct taskQueue> *tasks, int count, struct aperiodicTask aTasks[], double utilization)
{
    int lastDeadline = 0;
    struct taskQueue queueTask;
    int unitInterval = int(1 / utilization);

    for (int i = 0; i < count; i++)
    {
        queueTask.arrivalTime = aTasks[i].arrivalTime;
        queueTask.executionTime = aTasks[i].executionTime;
        queueTask.deadline = max(queueTask.arrivalTime, lastDeadline) + (queueTask.executionTime * unitInterval);
        queueTask.name = aTasks[i].name;
        lastDeadline = queueTask.deadline;
        tasks->push_back(queueTask);
    }
}

void addPeriodicTasks(vector<struct taskQueue> *tasks, int count, struct periodicTask pTasks[], int maxDeadline)
{
    for (int i = 0; i < count; i++)
    {
        int periodMark = 0;
        while (periodMark <= maxDeadline)
        {
            struct taskQueue queueTask;
            queueTask.arrivalTime = periodMark;
            periodMark += pTasks[i].period;
            queueTask.executionTime = pTasks[i].executionTime;
            queueTask.name = pTasks[i].name;
            queueTask.deadline = periodMark;
            tasks->push_back(queueTask);
        }
    }
}

void executeEDF(vector<struct taskQueue> taskList, vector<struct taskName> *output)
{
    int currentTime = 0;
    vector<struct taskQueue> readyQueue;

    struct taskName idleTask;
    idleTask.taskType = 'Z';
    idleTask.taskId = -1;

    while (!taskList.empty() || !readyQueue.empty())
    {
        while (!taskList.empty() && taskList.front().arrivalTime <= currentTime)
        {
            readyQueue.push_back(taskList.front());
            removeFirstTask(&taskList);
        }

        sort(readyQueue.begin(), readyQueue.end(), compareQueueDeadline);

        if (!readyQueue.empty())
        {
            output->push_back(readyQueue[0].name);
            readyQueue[0].executionTime -= 1;

            if (readyQueue[0].executionTime == 0)
                removeFirstTask(&readyQueue);
        }
        else
            output->push_back(idleTask);

        currentTime++;
    }
}

vector<struct taskQueue> calculateBandwidth(int periodicCount, struct periodicTask pTasks[], int aperiodicCount, struct aperiodicTask aTasks[], vector<struct taskName> *output)
{
    sort(pTasks, pTasks + periodicCount, comparePeriod);
    sort(aTasks, aTasks + aperiodicCount, compareArrival);

    double pUtil = calculateUtilization(periodicCount, pTasks);
    double sUtil = 1 - pUtil;

    vector<struct taskQueue> combinedTasks;

    addDeadlines(&combinedTasks, aperiodicCount, aTasks, sUtil);
    addPeriodicTasks(&combinedTasks, periodicCount, pTasks, combinedTasks.back().deadline);
    sort(combinedTasks.begin(), combinedTasks.end(), compareQueueArrival);

    executeEDF(combinedTasks, output);

    return combinedTasks;
}

