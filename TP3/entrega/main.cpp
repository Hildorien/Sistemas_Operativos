#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include "consola.hpp"
#include "nodo.hpp"

/* Variables globales. */

int np, rank; // Variables de MPI

int main(int argc, char *argv[]) {
    int status;

    // Inicializo MPI
    status = MPI_Init(&argc, &argv);
    if (status != MPI_SUCCESS){
        fprintf(stderr, "Error de MPI al inicializar.\n");
        MPI_Abort(MPI_COMM_WORLD, status);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Control del buffering: sin buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    printf("[MPI] Lanzando proceso %u\n", rank);

    if (rank == 0) {
        // Soy el proceso consola
        consola(np);
    }
    else {
        // Soy un nodo que procesa
        srand(time(NULL) + rank);
        nodo(rank);
    }

    // Limpio MPI
    MPI_Finalize();

    return 0;
}
