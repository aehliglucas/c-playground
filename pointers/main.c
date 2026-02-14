#include <stdio.h>

// Function prototype needed because birthday() is called before its definition
void birthday(int * age);

int main(void) {
    int age = 22;

    birthday(&age);

    printf("You are %d years old", age);
    return 0;
}

void birthday(int* age) {
    (*age)++;
}