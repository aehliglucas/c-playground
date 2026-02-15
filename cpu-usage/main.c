#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cpuUtilization
{
    unsigned long long user, nice, system, idle, iowait, irq, softirq;
};

struct cpuUtilization getCurrentUtilization(FILE* file)
{
    char* buffer[64];
    if (fgets(*buffer, 64, file) == NULL) {
        fprintf(stderr, "Can't read CPU data from buffer");
        exit(1);
    }
    strtok(*buffer, " ");

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
    };

    return currentUtilization;
}

int main(void) {
    FILE* file = fopen("/proc/stat", "r");

    if (file == NULL) {
        fprintf(stderr, "Can't open /proc/stat");
        return 1;
    }

    struct cpuUtilization currentUtilization = getCurrentUtilization(file);

    fclose(file);
    return 0;
}