#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define SIZE 10000000
#define MAX_NUM 1000

double getCurrentTime() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

void fillWithRandomNums(int* a, int size) {
    int i;
    for (int i = 0; i < size; i++) {
        a[i] = rand() % MAX_NUM;
    }
}

void printArray(int* a, int size) {
    int i;
    for (i = 0; i < size; i++ ) {
        printf("%04d", a[i]);
        if (i < size - 1) { printf(", "); }
    }
}

int intIncCmp(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

int main(int argc, char* argv[]) {

    srand(time(NULL));

    int* r_nums = malloc(SIZE * sizeof(int));

    fillWithRandomNums(r_nums, SIZE);

    double begin = getCurrentTime();

    qsort(r_nums, SIZE, sizeof(int), intIncCmp);

    double end = getCurrentTime();

    double proc_time = end - begin;

    printf("time: %f\n", proc_time);

    return 0;
}













