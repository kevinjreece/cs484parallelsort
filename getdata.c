#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <string.h>
#include <sys/time.h>

#define NUM_TIMES 1000
#define SIZE 65536

MPI_Status status;

double getCurrentTime() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

int main(int argc, char* argv[]) {
	srand(time(NULL));
	int proc_id;
    int n_proc;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    int num = 1;

    double begin = getCurrentTime();

    int i;
    for (i = 0; i < NUM_TIMES; i++) {
    	if (proc_id == 0) {
    		MPI_Send(&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);// Send
    	}
    	if (proc_id == 1) {
    		MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);// Receive
    	}
    }

    double end = getCurrentTime();
    double run_time = end - begin;
    double latency = run_time / NUM_TIMES;
    if (proc_id == 0) {
	    printf("1st time: %.10f\n", run_time);
	    printf("latency: %.10f\n", latency);
	}

    int* nums = malloc(SIZE * sizeof(int));
    memset(nums, 4, SIZE);

    begin = getCurrentTime();

    if (proc_id == 0) {
		MPI_Send(nums, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);// Send
	}
	if (proc_id == 1) {
		MPI_Recv(nums, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);// Receive
	}

	end = getCurrentTime();
    run_time = end - begin;
    if (proc_id == 0) {
	    printf("2nd time: %.10f\n", run_time);
	    printf("bandwidth: %.10f\n", (SIZE * sizeof(int)) / (run_time - latency));
	}

	free(nums);

    MPI_Finalize();
}