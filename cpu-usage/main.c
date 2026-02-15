#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Logic to catch SIGINT to close the file handler
volatile sig_atomic_t g_running = true;
void intHandler(int signum)
{
    g_running = false;
}

struct cpuUtilization
{
    long long user, nice, system, idle, iowait, irq, softirq;
};

struct cpuUtilization previousUtilization = {
    0, 0, 0, 0, 0, 0, 0
};

struct cpuUtilization getCurrentUtilization(FILE* file)
{
    char buffer[256];
    if (fgets(buffer, 256, file) == NULL) {
        fprintf(stderr, "Can't read CPU data from buffer");
        fclose(file);
        exit(1);
    }
    strtok(buffer, " ");

    long long cpuData[64];
    int count = 0;
    char* dataToken;

    while ((dataToken = strtok(NULL, " ")) != NULL && count < 64) {
        cpuData[count] = atoll(dataToken);
        count++;
    }

    struct cpuUtilization currentUtilization = {
        cpuData[0],
        cpuData[1],
        cpuData[2],
        cpuData[3],
        cpuData[4],
        cpuData[5],
        cpuData[6],
    };

    return currentUtilization;
}

struct utilizationTimeDelta
{
    long long deltaWorkTime;
    long long deltaTotalTime;
};

struct utilizationTimeDelta calculateTimeDelta(struct cpuUtilization currentUtilization)
{
    struct utilizationTimeDelta result;
    long long oldWorkTime = previousUtilization.user + previousUtilization.nice + previousUtilization.system + previousUtilization.irq + previousUtilization.softirq;
    long long oldTotalTime = oldWorkTime + previousUtilization.idle + previousUtilization.iowait;
    long long newWorkTime = currentUtilization.user + currentUtilization.nice + currentUtilization.system + currentUtilization.irq + currentUtilization.softirq;
    long long newTotalTime = newWorkTime + currentUtilization.idle + currentUtilization.iowait;

    result.deltaWorkTime = llabs(newWorkTime - oldWorkTime);
    result.deltaTotalTime = llabs(newTotalTime - oldTotalTime);

    return result;
};

int main(void) {
    FILE* file = fopen("/proc/stat", "r");
    // Disable C-internal file buffering for /proc/stat
    setvbuf(file, NULL, _IONBF, 0);

    if (file == NULL) {
        fprintf(stderr, "Can't open /proc/stat");
        return 1;
    }

    while (g_running)
    {
        rewind(file);
        struct cpuUtilization currentUtilization = getCurrentUtilization(file);
        struct utilizationTimeDelta timeDelta = calculateTimeDelta(currentUtilization);
        if (timeDelta.deltaWorkTime == 0 || timeDelta.deltaTotalTime == 0)
        {
            continue;
        }
        printf("\r%3lld   ", 100 * timeDelta.deltaWorkTime / timeDelta.deltaTotalTime);
        fflush(stdout);
        previousUtilization = currentUtilization;
        sleep(1);
    }

    fclose(file);
    return 0;
}