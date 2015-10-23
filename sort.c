#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "mpi.h"

#define SIZE 32
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

int intIncCmp(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

int intDecCmp(const void *a, const void *b) {
    return *(int*)b - *(int*)a;
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

    int next_open = SIZE / n_proc;
    int dims = log2(n_proc);
    MPI_Comm comms[dims + 1];
    int rank[dims];
    int world_size[dims];
    int pivot[dims];
    int group_id[dims];
    int partner[dims];
    int* r_nums = malloc(SIZE * sizeof(int));;

    fillWithRandomNums(r_nums, next_open);

    // printf("process %d orignal values: \t", proc_id);
    // printArray(r_nums, SIZE);
    // printf("\n");

    // qsort(r_nums, next_open, sizeof(int), intIncCmp);

    // printf("process %d sorted values: \t", proc_id);
    // printArray(r_nums, SIZE);
    // printf("\n");

    comms[0] = MPI_COMM_WORLD;

    int i;
    for (i = 0; i < dims; i++) {
        /* get data for world */
    	MPI_Comm_size(comms[i], &world_size[i]);
    	MPI_Comm_rank(comms[i], &rank[i]);
        int group_size = world_size[i] / 2;

        if (proc_id == 1) {
            printf("\ndim: %d\n", i);
        }

        /* get pivot */
		// pivot[i] = proc_id;
		int pivot_delta = MAX_NUM >> (i + 1);// value to be added or subtracted from previous pivot
		pivot[i] = (i == 0 ? MAX_NUM / 2 : pivot[i - 1] + (rank[i - 1] / world_size[i] ? pivot_delta : -pivot_delta));//

        /* broadcast pivot to world (if needed) */
        // MPI_Bcast(&pivot[i], 1, MPI_INT, 0, comms[i]);

        /* figure out partner and group */
        group_id[i] = rank[i] < group_size ? 0 : 1;
        partner[i] = rank[i] + (group_id[i] == 0 ? group_size : -group_size);

        /* decide what data to send to partner and exchange data */
        if (group_id[i] == 0) {
            // sort data increasing order
            qsort(r_nums, next_open, sizeof(int), intIncCmp);
            // from right, go left until find smallest element greater than pivot
            int send_idx = next_open;
            while (send_idx > 0 && r_nums[send_idx - 1] > pivot[i]) { send_idx--; }
            // send to partner
            // receive from partner
            printf("dim: %d\tp_id: %d\trank: %d\tpivot: %d\tgroup: %d\tpartner: %d\tsplit at %d.", i, proc_id, rank[i], pivot[i], group_id[i], partner[i], r_nums[send_idx]);
            printf("\tINC sorted values: ");
            printArray(r_nums, next_open);
            printf("\n");
            printf("dim: %d\tp_id: %d\tvalues to send: ", i, proc_id);
            printArray(&r_nums[send_idx], next_open - send_idx);
            printf("\n");
        }
        else if (group_id[i] == 1) {
            // sort data decreasing order
            qsort(r_nums, next_open, sizeof(int), intDecCmp);
            // from right, go left until find last element less than or equal to pivot
            int send_idx = next_open;
            while (send_idx > 0 && r_nums[send_idx - 1] <= pivot[i]) { send_idx--; }
            // receive from partner
            // send to partner
            printf("dim: %d\tp_id: %d\trank: %d\tpivot: %d\tgroup: %d\tpartner: %d\tsplit at %d.", i, proc_id, rank[i], pivot[i], group_id[i], partner[i], r_nums[send_idx]);
            printf("\tDEC sorted values: ");
            printArray(r_nums, next_open);
            printf("\n");
            printf("dim: %d\tp_id: %d\tvalues to send: ", i, proc_id);
            printArray(&r_nums[send_idx], next_open - send_idx);
            printf("\n");
        }
        else { printf("\n\nERROR\n\n"); return 1; }

	    MPI_Comm_split(comms[i], group_id[i], rank[i], &comms[i + 1]);

	}

    MPI_Finalize();

    return 0;
}













