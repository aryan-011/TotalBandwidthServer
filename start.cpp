//  g++ start.cpp -o start -lallegro -lallegro_primitives
#include <bits/stdc++.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h> // Optional, for TTF font support
#include "function_str.cpp"
#include "gantt_chart.cpp"

using namespace std;

PerformanceMetrics calculatePerformanceMetrics(
    vector<struct taskName> taskOutput,
    vector<struct taskQueue> taskList,
    periodicTask *periodicTasks,
    int periodicCount,
    int aperiodicCount)
{
    PerformanceMetrics metrics = {0};

    // Total simulation time
    int simulationTime = taskOutput.size();

    // Aperiodic Task Response Time
    vector<int> aperiodicResponseTimes;

    for (auto &task : taskList)
    {
        if (task.name.taskType == 'A')
        {
            // Find when this specific aperiodic task ends
            int taskEndTime = -1;
            for (int i = 0; i < taskOutput.size(); i++)
            {
                if (taskOutput[i].taskType == 'A' &&
                    taskOutput[i].taskId == task.name.taskId)
                {
                    taskEndTime = i;
                }
            }

            // Calculate response time if task was found
            if (taskEndTime != -1)
            {
                int responseTime = taskEndTime - task.arrivalTime;
                aperiodicResponseTimes.push_back(responseTime);
            }
        }
    }

    // Calculate average response time
    double totalResponseTime = 0;
    for (int rt : aperiodicResponseTimes)
    {
        totalResponseTime += rt;
    }

    metrics.aperiodicResponseTime = aperiodicResponseTimes.empty() ? 0 : (totalResponseTime / aperiodicResponseTimes.size());

    // Periodic Task Utilization
    int periodicExecutionTime = 0;
    for (auto &task : taskOutput)
    {
        if (task.taskType == 'P')
        {
            periodicExecutionTime++;
        }
    }
    metrics.periodicUtilization = (double)calculateUtilization(periodicCount, periodicTasks) * 100;

    // Server Utilization
    int idleTicks = 0;
    for (auto &task : taskOutput)
    {
        if (task.taskId == -1)
        { // Idle task
            idleTicks++;
        }
    }

    metrics.serverUtilization = 100 - metrics.periodicUtilization;

    return metrics;
}
void renderGanttChart(vector<struct taskName> taskOutput, vector<struct taskQueue> taskList, int periodicTaskCount, int aperiodicTaskCount, periodicTask *periodicTasks)
{
    ALLEGRO_DISPLAY *chartDisplay = NULL;
    if (!al_init())
        return;

    al_init_primitives_addon();
    al_init_font_addon();
    chartDisplay = al_create_display(750, 500);
    al_clear_to_color(al_map_rgb(255, 255, 255));
    if (!chartDisplay)
        return;

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
                chartRenderer.renderTaskBlock((timeStep + 1) * 25, verticalOffset + (100 * (taskOutput[timeStep].taskId - 1)) - 50, (timeStep + 2) * 25, verticalOffset + (100 * (taskOutput[timeStep].taskId - 1)), 'P', ("P" + to_string(taskOutput[timeStep].taskId)).c_str());
            }
            else if (taskOutput[timeStep].taskType == 'A')
            {
                chartRenderer.renderTaskBlock((timeStep + 1) * 25, verticalOffset + (100 * periodicTaskCount) - 50, (timeStep + 2) * 25, verticalOffset + (100 * periodicTaskCount), 'A', ("A" + to_string(taskOutput[timeStep].taskId)).c_str());
            }
        }

        PerformanceMetrics metrics = calculatePerformanceMetrics(taskOutput, taskList, periodicTasks, periodicTaskCount, aperiodicTaskCount);

        // Render metrics text
        ALLEGRO_FONT *font = al_create_builtin_font();

        // Prepare and draw individual metric lines
        char responseTimeText[100];
        char periodicUtilText[100];
        char serverUtilText[100];

        snprintf(responseTimeText, sizeof(responseTimeText),
                 "Aperiodic Avg Response Time: %.2f",
                 metrics.aperiodicResponseTime);

        snprintf(periodicUtilText, sizeof(periodicUtilText),
                 "Periodic Task Utilization: %.2f%%",
                 metrics.periodicUtilization);

        snprintf(serverUtilText, sizeof(serverUtilText),
                 "Server Utilization: %.2f%%",
                 metrics.serverUtilization);

        // Draw metrics at the bottom of the chart
        int bottomY = 350; // Adjust based on your chart height
        al_draw_text(font, al_map_rgb(0, 0, 0), 50, bottomY, ALLEGRO_ALIGN_LEFT, responseTimeText);
        al_draw_text(font, al_map_rgb(0, 0, 0), 50, bottomY + 20, ALLEGRO_ALIGN_LEFT, periodicUtilText);
        al_draw_text(font, al_map_rgb(0, 0, 0), 50, bottomY + 40, ALLEGRO_ALIGN_LEFT, serverUtilText);

        // Cleanup
        al_destroy_font(font);
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
    double util = calculateUtilization(periodicCount, periodicTasks);
    if (util > 1)
    {
        cout << "Task Set not schedulable since the periodics task utilizations exceed 1 and is " << util;
        ALLEGRO_DISPLAY *chartDisplay = NULL;
        if (!al_init())
            return 0;

        al_init_primitives_addon();
        al_init_font_addon();
        chartDisplay = al_create_display(750, 500);
        if (!chartDisplay)
        {
            printf("Failed to create display!\n");
            return 0;
        }
        al_clear_to_color(al_map_rgb(0, 0, 0));

        // Display the message
        ALLEGRO_FONT *font = al_create_builtin_font();
        al_draw_text(font, al_map_rgb(255, 0, 0), 200, 100, ALLEGRO_ALIGN_CENTER, "Not Schedulable");
        al_flip_display();

        // Keep the window open for 3 seconds
        al_rest(3.0);
        return 0;
    }
    combinedTasks = calculateBandwidth(periodicCount, periodicTasks, aperiodicCount, aperiodicTasks, &outputTasks);

    renderGanttChart(outputTasks, combinedTasks, periodicCount, aperiodicCount, periodicTasks);

    return 0;
}
