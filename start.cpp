//  g++ start.cpp -o start -lallegro -lallegro_primitives
#include <bits/stdc++.h>
#include "function_str.cpp"
#include "gantt_chart.cpp"

using namespace std;

void renderGanttChart(vector<struct taskName> taskOutput, vector<struct taskQueue> taskList, int periodicTaskCount, int aperiodicTaskCount)
{
    ALLEGRO_DISPLAY *chartDisplay = NULL;
    if (!al_init()) return;

    al_init_primitives_addon();
    chartDisplay = al_create_display(750, 500);
    al_clear_to_color(al_map_rgb(255, 255, 255));
    if (!chartDisplay) return;

    GanttChartRenderer chartRenderer;
    int verticalOffset = 100;

    for (int currentTime = 0; currentTime < int(taskOutput.size()); currentTime++)
    {
        al_clear_to_color(al_map_rgb(255, 255, 255));

        for (int p = 0; p < (periodicTaskCount + 1); p++)
        {
            chartRenderer.renderLine(verticalOffset + (100 * p));
        }

        for (int timeStep = 0; timeStep <= currentTime; timeStep++)
        {
            int idx = 0;
            while (taskList[idx].arrivalTime <= timeStep)
            {
                if (taskList[idx].name.taskType == 'P')
                    chartRenderer.markTaskRelease((taskList[idx].arrivalTime + 1) * 25, verticalOffset + (100 * (taskList[idx].name.taskId - 1)));
                else if (taskList[idx].name.taskType == 'A')
                    chartRenderer.markTaskRelease((taskList[idx].arrivalTime + 1) * 25, verticalOffset + (100 * periodicTaskCount));
                idx++;
            }

            idx = 0;
            while (idx <= int(taskList.size()))
            {
                if (taskList[idx].deadline <= timeStep)
                {
                    if (taskList[idx].name.taskType == 'P')
                        chartRenderer.markDeadline((taskList[idx].deadline + 1) * 25, verticalOffset + (100 * (taskList[idx].name.taskId - 1)));
                    else if (taskList[idx].name.taskType == 'A')
                        chartRenderer.markDeadline((taskList[idx].deadline + 1) * 25, verticalOffset + (100 * periodicTaskCount));
                }
                idx++;
            }

            if (taskOutput[timeStep].taskType == 'P')
            {
                chartRenderer.renderTaskBlock((timeStep + 1) * 25, verticalOffset + (100 * (taskOutput[timeStep].taskId - 1)) - 50, (timeStep + 2) * 25, verticalOffset + (100 * (taskOutput[timeStep].taskId - 1)), 'P');
            }
            else if (taskOutput[timeStep].taskType == 'A')
            {
                chartRenderer.renderTaskBlock((timeStep + 1) * 25, verticalOffset + (100 * periodicTaskCount) - 50, (timeStep + 2) * 25, verticalOffset + (100 * periodicTaskCount), 'A');
            }
        }

        al_flip_display();
        al_rest(0.25);
    }

    // Keep the display open until the user closes it
    ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
    al_register_event_source(eventQueue, al_get_display_event_source(chartDisplay));
    bool running = true;

    while (running)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = false;
        }
    }

    al_destroy_event_queue(eventQueue);
    al_destroy_display(chartDisplay);
}

int main()
{
    ifstream inputFile("input.txt");
    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open input file.\n";
        return 1;
    }

    int periodicCount, aperiodicCount;
    inputFile >> periodicCount;

    struct periodicTask periodicTasks[periodicCount];
    for (int t = 0; t < periodicCount; t++)
    {
        inputFile >> periodicTasks[t].executionTime >> periodicTasks[t].period;
        periodicTasks[t].name.taskType = 'P';
        periodicTasks[t].name.taskId = t + 1;
    }

    inputFile >> aperiodicCount;
    struct aperiodicTask aperiodicTasks[aperiodicCount];
    for (int t = 0; t < aperiodicCount; t++)
    {
        inputFile >> aperiodicTasks[t].arrivalTime >> aperiodicTasks[t].executionTime;
        aperiodicTasks[t].name.taskType = 'A';
        aperiodicTasks[t].name.taskId = t + 1;
    }

    inputFile.close();

    vector<struct taskQueue> combinedTasks;
    vector<struct taskName> outputTasks;
    combinedTasks = calculateBandwidth(periodicCount, periodicTasks, aperiodicCount, aperiodicTasks, &outputTasks);

    renderGanttChart(outputTasks, combinedTasks, periodicCount, aperiodicCount);

    return 0;
}
