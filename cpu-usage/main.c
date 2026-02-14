#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cpuUtilization
{
    long long user;
    long long nice;
    long long system;
    long long idle;
    long long iowait;
};

struct cpuUtilization getCurrentUtilization(FILE* file)
{
    char* buffer[64];
    if (fgets(*buffer, 64, file) == NULL) {
        printf("%s", "whatever");
    }
    strtok(*buffer, " ");

    long long cpuData[10];
    int count = 0;
    char* dataToken;

    while ((dataToken = strtok(NULL, " ")) != NULL && count < 10) {
        cpuData[count] = atoll(dataToken);
        count++;
    }

    struct cpuUtilization currentUtilization = {
        cpuData[0],
        cpuData[1],
        cpuData[2],
        cpuData[3],
        cpuData[4]
    };

    return currentUtilization;
}

int main(void) {
    FILE* file = fopen("/proc/stat", "r");

    if (file == NULL) {
        fprintf(stderr, "Can't open /proc/stat");
        return 1;
    }

    struct cpuUtilization utilization = getCurrentUtilization(file);
    printf("User:%lld\n", utilization.user);

    fclose(file);
    return 0;
}