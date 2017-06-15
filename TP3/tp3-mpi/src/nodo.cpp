#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void nodo(unsigned int rank) {
    printf("Soy un nodo. Mi rank es %d \n", rank);

    // TODO: Implementar
    // Creo un HashMap loal

    while (true) {
        // TODO: Procesar mensaje
    }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
