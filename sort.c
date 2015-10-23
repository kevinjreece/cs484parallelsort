#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "mpi.h"

#define SIZE 16
#define MAX_NUM 1000

void fillWithRandomNums(int* a, int size) {
    int i;
    for (int i = 0; i < size; i++) {
        a[i] = rand() % MAX_NUM;
    }
}

void printArray(int* a, int size) {
    int i;
    for (i = 0; i < size; i++ ) {
        printf("%d", a[i]);
        if (i < size - 1) { printf(", "); }
    }
}

int intCmp(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

int main(int argc, char* argv[]) {
	// for each dimension:
		// find world pivot
		// split data
		// find partner in world
		// send data to partner
		// receive data from partner

    srand(time(NULL));
    int proc_id;
    int n_proc;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    int dims = log2(n_proc);
    MPI_Comm comms[dims + 1];
    int rank[dims];
    int world_size[dims];
    int pivot[dims];
    int group_id[dims];
    int partner[dims];
    int* r_nums = malloc(SIZE * sizeof(int));;

    fillWithRandomNums(r_nums, SIZE);

    // printf("process %d orignal values: \t", proc_id);
    // printArray(r_nums, SIZE);
    // printf("\n");

    qsort(r_nums, SIZE, sizeof(int), intCmp);

    // printf("process %d sorted values: \t", proc_id);
    // printArray(r_nums, SIZE);
    // printf("\n");

    comms[0] = MPI_COMM_WORLD;

    int i;
    for (i = 0; i < dims; i++) {
    	MPI_Comm_size(comms[i], &world_size[i]);
    	MPI_Comm_rank(comms[i], &rank[i]);

		// pivot[i] = proc_id;
		int pivot_delta = MAX_NUM >> (i + 1);// value to be added or subtracted from previous pivot
		pivot[i] = (i == 0 ? MAX_NUM / 2 : pivot[i - 1] + (rank[i - 1] / world_size[i] ? pivot_delta : -pivot_delta));//

    	MPI_Bcast(&pivot[i], 1, MPI_INT, 0, comms[i]);

    	int group_size = world_size[i] / 2;

	    group_id[i] = rank[i] < group_size ? 0 : 1;
	    partner[i] = rank[i] + (group_id[i] == 0 ? group_size : -group_size);

    	printf("dim: %d\tid: %d\trank: %d\tpivot: %d\tgroup: %d\tpartner: %d\n", i, proc_id, rank[i], pivot[i], group_id[i], partner[i]);
     

	    MPI_Comm_split(comms[i], group_id[i], rank[i], &comms[i + 1]);

	}

    MPI_Finalize();

    return 0;
}













