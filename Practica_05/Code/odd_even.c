/*
 * Run:
 *    mpiexec -n <p> parallel_odd_even <g|i> <global_n>
 *       - p: the number of processes
 *       - g: generate random, distributed list
 *       - i: user will input list on process 0
 *       - global_n: number of elements in global list
 *
 * Notes:
 * 1.  global_n must be evenly divisible by p
 * 2.  DEBUG flag prints original and final sublists
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>       /* time */
 // const int RMAX = 1000000000;
const int RMAX = 100;

/* Local functions */
void Usage(char* program);
void Print_list(int local_A[], int local_n, int rank);
void Merge_split_low(int local_A[], int temp_B[], int temp_C[],
    int local_n);
void Merge_split_high(int local_A[], int temp_B[], int temp_C[],
    int local_n);
void Generate_list(int local_A[], int local_n, int my_rank);
int  Compare(const void* a_p, const void* b_p);

/* Functions involving communication */
void Get_args(int argc, char* argv[], int* global_n_p, int* local_n_p,
    char* gi_p, int my_rank, int p, MPI_Comm comm);
void Sort(int local_A[], int local_n, int my_rank,
    int p, MPI_Comm comm);
void Odd_even_iter(int local_A[], int temp_B[], int temp_C[],
    int local_n, int phase, int even_partner, int odd_partner,
    int my_rank, int p, MPI_Comm comm);
void Print_local_lists(int local_A[], int local_n,
    int my_rank, int p, MPI_Comm comm);
void Print_global_list(int local_A[], int local_n, int my_rank,
    int p, MPI_Comm comm);


int main(int argc, char* argv[]) {
    int my_rank, p;
    char g_i;
    int* local_A;
    int global_n;
    int local_n;
    MPI_Comm comm;
    double start, finish;

    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &my_rank);

    Get_args(argc, argv, &global_n, &local_n, &g_i, my_rank, p, comm);
    local_A = (int*)malloc(local_n * sizeof(int));
    if (g_i == 'g') {
        Generate_list(local_A, local_n, my_rank);
    }
    else {
        //Read_list(local_A, local_n, my_rank, p, comm);
    }
#  ifdef DEBUG
    Print_local_lists(local_A, local_n, my_rank, p, comm);
#  endif

    start = MPI_Wtime();
    Sort(local_A, local_n, my_rank, p, comm);
    finish = MPI_Wtime();
    if (my_rank == 0)
        printf("Elapsed time = %e seconds\n", finish - start);

#  ifdef DEBUG
    Print_local_lists(local_A, local_n, my_rank, p, comm);
    fflush(stdout);
#  endif

    Print_global_list(local_A, local_n, my_rank, p, comm);

    free(local_A);

    MPI_Finalize();

    return 0;
}  /* main */

void Generate_list(int local_A[], int local_n, int my_rank) {
    int i;

    srand(my_rank + 1);
    for (i = 0; i < local_n; i++)
        local_A[i] = rand() % RMAX;

}  /* Generate_list */


void Usage(char* program) {
    fprintf(stderr, "usage:  mpirun -np <p> %s <g|i> <global_n>\n",
        program);
    fprintf(stderr, "   - p: the number of processes \n");
    fprintf(stderr, "   - g: generate random, distributed list\n");
    fprintf(stderr, "   - i: user will input list on process 0\n");
    fprintf(stderr, "   - global_n: number of elements in global list");
    fprintf(stderr, " (must be evenly divisible by p)\n");
    fflush(stderr);
}  /* Usage */


/*-------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get and check command line arguments
 * Input args:  argc, argv, my_rank, p, comm
 * Output args: global_n_p, local_n_p, gi_p
 */
void Get_args(int argc, char* argv[], int* global_n_p, int* local_n_p,
    char* gi_p, int my_rank, int p, MPI_Comm comm) {

    if (my_rank == 0) {
        if (argc != 3) {
            Usage(argv[0]);
            *global_n_p = -1;  /* Bad args, quit */
        }
        else {
            *gi_p = argv[1][0];
            if (*gi_p != 'g' && *gi_p != 'i') {
                Usage(argv[0]);
                *global_n_p = -1;  /* Bad args, quit */
            }
            else {
                *global_n_p = strtol(argv[2], NULL, 10);
                if (*global_n_p % p != 0) {
                    Usage(argv[0]);
                    *global_n_p = -1;
                }
            }
        }
    }  /* my_rank == 0 */

    MPI_Bcast(gi_p, 1, MPI_CHAR, 0, comm);
    MPI_Bcast(global_n_p, 1, MPI_INT, 0, comm);

    if (*global_n_p <= 0) {
        MPI_Finalize();
        exit(-1);
    }

    *local_n_p = *global_n_p / p;

}  /* Get_args */



void Print_global_list(int local_A[], int local_n, int my_rank, int p,
    MPI_Comm comm) {
    int* A = NULL;
    int i, n;

    if (my_rank == 0) {
        n = p * local_n;
        A = (int*)malloc(n * sizeof(int));
        MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0,
            comm);
        printf("Global list:\n");
        for (i = 0; i < n; i++)
            printf("%d ", A[i]);
        printf("\n\n");
        free(A);
    }
    else {
        MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0,
            comm);
    }

}  /* Print_global_list */


int Compare(const void* a_p, const void* b_p) {
    int a = *((int*)a_p);
    int b = *((int*)b_p);

    if (a < b)
        return -1;
    else if (a == b)
        return 0;
    else /* a > b */
        return 1;
}  /* Compare */


void Sort(int local_A[], int local_n, int my_rank,
    int p, MPI_Comm comm) {
    int phase;
    int* temp_B, * temp_C;
    int even_partner;  /* phase is even or left-looking */
    int odd_partner;   /* phase is odd or right-looking */

    /* Temporary storage used in merge-split */
    temp_B = (int*)malloc(local_n * sizeof(int));
    temp_C = (int*)malloc(local_n * sizeof(int));

    /* Find partners:  negative rank => do nothing during phase */
    if (my_rank % 2 != 0) {
        even_partner = my_rank - 1;
        odd_partner = my_rank + 1;
        if (odd_partner == p) odd_partner = -1;  // Idle during odd phase
    }
    else {
        even_partner = my_rank + 1;
        if (even_partner == p) even_partner = -1;  // Idle during even phase
        odd_partner = my_rank - 1;
    }

    /* Sort local list using built-in quick sort */
    qsort(local_A, local_n, sizeof(int), Compare);

    for (phase = 0; phase < p; phase++)
        Odd_even_iter(local_A, temp_B, temp_C, local_n, phase,
            even_partner, odd_partner, my_rank, p, comm);

    free(temp_B);
    free(temp_C);
}  /* Sort */


void Odd_even_iter(int local_A[], int temp_B[], int temp_C[],
    int local_n, int phase, int even_partner, int odd_partner,
    int my_rank, int p, MPI_Comm comm) {
    MPI_Status status;

    if (phase % 2 == 0) {  /* Even phase, odd process <-> rank-1 */
        if (even_partner >= 0) {
            MPI_Sendrecv(local_A, local_n, MPI_INT, even_partner, 0,
                temp_B, local_n, MPI_INT, even_partner, 0, comm,
                &status);
            if (my_rank % 2 != 0)
                Merge_split_high(local_A, temp_B, temp_C, local_n);
            else
                Merge_split_low(local_A, temp_B, temp_C, local_n);
        }
    }
    else { /* Odd phase, odd process <-> rank+1 */
        if (odd_partner >= 0) {
            MPI_Sendrecv(local_A, local_n, MPI_INT, odd_partner, 0,
                temp_B, local_n, MPI_INT, odd_partner, 0, comm,
                &status);
            if (my_rank % 2 != 0)
                Merge_split_low(local_A, temp_B, temp_C, local_n);
            else
                Merge_split_high(local_A, temp_B, temp_C, local_n);
        }
    }
}  /* Odd_even_iter */



void Merge_split_low(int local_A[], int temp_B[], int temp_C[],
    int local_n) {
    int ai, bi, ci;

    ai = 0;
    bi = 0;
    ci = 0;
    while (ci < local_n) {
        if (local_A[ai] <= temp_B[bi]) {
            temp_C[ci] = local_A[ai];
            ci++; ai++;
        }
        else {
            temp_C[ci] = temp_B[bi];
            ci++; bi++;
        }
    }

    memcpy(local_A, temp_C, local_n * sizeof(int));
}  /* Merge_split_low */


void Merge_split_high(int local_A[], int temp_B[], int temp_C[],
    int local_n) {
    int ai, bi, ci;

    ai = local_n - 1;
    bi = local_n - 1;
    ci = local_n - 1;
    while (ci >= 0) {
        if (local_A[ai] >= temp_B[bi]) {
            temp_C[ci] = local_A[ai];
            ci--; ai--;
        }
        else {
            temp_C[ci] = temp_B[bi];
            ci--; bi--;
        }
    }

    memcpy(local_A, temp_C, local_n * sizeof(int));
}  /* Merge_split_low */


void Print_list(int local_A[], int local_n, int rank) {
    int i;
    printf("%d: ", rank);
    for (i = 0; i < local_n; i++)
        printf("%d ", local_A[i]);
    printf("\n");
}  /* Print_list */


void Print_local_lists(int local_A[], int local_n,
    int my_rank, int p, MPI_Comm comm) {
    int* A;
    int        q;
    MPI_Status status;

    if (my_rank == 0) {
        A = (int*)malloc(local_n * sizeof(int));
        Print_list(local_A, local_n, my_rank);
        for (q = 1; q < p; q++) {
            MPI_Recv(A, local_n, MPI_INT, q, 0, comm, &status);
            Print_list(A, local_n, q);
        }
        free(A);
    }
    else {
        MPI_Send(local_A, local_n, MPI_INT, 0, 0, comm);
    }
}  /* Print_local_lists */
