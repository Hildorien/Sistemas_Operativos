#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void nodo(unsigned int rank) {
    printf("Soy un nodo. Mi rank es %d \n", rank);

    // TODO: Implementar
    // Creo un HashMap local
    HashMap MiHashMap = HashMap();
    MPI_Status status;
    int SOURCE = 0;
    int count = 4;
    MPI_CHAR* buf;

    while (true) {
        // TODO: Procesar mensaje
    	
    	//Probe intercepta mensaje y guarda el TAG, el TAMAÑO DEL MENSAJE y el RANK DEL EMISOR
    	//Se lee: espero un mensaje de CUALQUIER TAG (el tag define la funcion que me llaman) y del nodo 0 (consola).
    	MPI_Probe(SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	   
	    //Declaramos una variable para guardar la cantidad de elementos de un MPI_DATA_TYPE del mensaje
	    int msjcount;

	    //No podemos acceder tan facil al tamaño del mensaje. Hay que llamar a MPI_Get_count, donde:
	    //Status= status de referencia
	    //MPI_CHAR = el tipo de datos que va a buscar
	    //msjSize = Donde va a guardar la cantidad de (MPI_DATA_TYPE) del mensaje.
    	MPI_Get_count(&status, MPI_CHAR, &msjcount);

    	//Pido un buffer de memoria del tamaño del mensaje
    	buf = malloc(msjcount * sizeof(MPI_CHAR));

    	//Leo el tag y identifico que funcion tengo que correr.
    	int funcion = status.MPI_TAG;

    	/*funcion = 1 ==> load()
		  funcion = 2 ==> addAndInc()
		  funcion = 3 ==> member()
		  funcion = 4 ==> Maximum()
		  funcion = 5 ==> Quit()
		*/

    	if (funcion == 1){
    		/* LOAD */
    		MPI_Recv(&buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);

    		//Estamos en condiciones de empezar LOAD. En buff tenemos los parametros. 
    		miHashMap.load(&buf);
    		trabajarArduamente();

    		free(buf);
    		int* soyRank;

    		soyRank =(MPI_INT*) malloc(MPI_INT);
    		soyRank[0] = rank;
    		//Respuesta OK bloqueante.
    		MPI_Send(&soyRank,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);
    		free(soyRank);

    	} else if (TRUE/*TODO*/){ /*TODO: RESTO DE LAS FUNCIONES*/}

	    	
    	
        



    }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
