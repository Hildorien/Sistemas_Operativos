#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <ctype.h>
#include <utility>

using namespace std;

void nodo(unsigned int rank) {
    printf("Soy un nodo. Mi rank es %d \n", rank);

    // TODO: Implementar
    // Creo un HashMap local
    HashMap miHashMap;
    MPI_Status status;
    int SOURCE = 0;
    
    while (true) {
        // TODO: Procesar mensaje
    	
    	//MPI_Probe intercepta mensaje y guarda el TAG, el TAMAÑO DEL MENSAJE y el RANK DEL EMISOR
    	//Se lee: espero un mensaje del nodo 0 (consola/SOURCE), y de CUALQUIER TAG (el tag define la funcion que me llaman)
    	MPI_Probe(SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	   
	    //Declaramos una variable para guardar la cantidad de elementos de un MPI_DATA_TYPE del mensaje
	    int msjcount;

	    //No podemos acceder tan facil al tamaño del mensaje. Hay que llamar a MPI_Get_count, donde:
	    //Status= status de referencia
	    //MPI_CHAR = el tipo de datos que va a buscar
	    //msjSize = Donde va a guardar la cantidad de (MPI_DATA_TYPE) del mensaje.
    	MPI_Get_count(&status, MPI_CHAR, &msjcount);

    	//Pido un buffer de memoria del tamaño del mensaje
        
    	//Leo el tag e identifico que funcion tengo que correr.
    	int funcion = status.MPI_TAG;

    	/*funcion = 1 ==> load()
		  funcion = 2 ==> addAndInc()
		  funcion = 3 ==> member()
		  funcion = 4 ==> Maximum()
		  funcion = 5 ==> Quit()
		*/
    	
        if (funcion == 1){
    		/* LOAD */
    		//Recepcion bloqueante
            char buf[msjcount];
        
    		MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);
    		
            cout << "Soy el nodo " << rank << " y cargue el archivo " << buf << endl;
            //Estamos en condiciones de empezar LOAD. En buff tenemos los parametros. 
    		miHashMap.load(buf);
    		trabajarArduamente();
    		
    		int soyRank = rank;

            //Respuesta OK bloqueante.
    		//Se lee: Pasar el contenido que puse en &SOYRANK, 1 elemento del tipo, MPI_INT, destinado a SOURCE, con el tag 99, y furta_comm_world
    		
            MPI_Send(&soyRank,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);
    		
    	} else if (funcion == 2){ 
    		/*ADD AND INC*/
 
            int bufi; 
    		int tamMsj;
    		char buf[msjcount];

            //Primer Recv. Los nodos se enteran que hay para hacer un addAndInc
    		MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);
			
            MPI_Send(&bufi,1,MPI_INT,SOURCE,99,MPI_COMM_WORLD);
    		
            //Me quedo esperando a la confirmacion de source (si tengo que hacer addAndInc o no)
			MPI_Recv(&bufi,1,MPI_INT,SOURCE,funcion,MPI_COMM_WORLD,&status);
            
            if ( bufi == 1)
			{
				
                //Recibi un 1. Entonces me toca trabajar. Me quedo esperando a que me mande el string
				MPI_Probe(SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Get_count(&status, MPI_CHAR, &tamMsj);

				char palabra[tamMsj];

				MPI_Recv(palabra,tamMsj,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);
				//buf[tamMsj] = 0;
                cout << "Soy el nodo " << rank << " y agregue la palabra " << palabra << endl;
                miHashMap.addAndInc(palabra);
                
                trabajarArduamente();
               
			}

            //Respuesta OK bloqueante.
            //Se lee: Pasar el contenido que puse en &SOYRANK, 1 elemento del tipo, MPI_INT, destinado a SOURCE, con el tag 99, y furta_comm_world
            
            MPI_Send(&rank,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);

    	 }else if ( funcion == 3) {
            //Member

            int bufi;

            char buf[msjcount];
            //Primer recieved. Los nodos se enteran de que tienen que hacer member
            MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);
            
            bool res = miHashMap.member(buf);
            trabajarArduamente();
            
            if (res) 
            {
                bufi = 1;
                MPI_Send(&bufi,1,MPI_INT,SOURCE,99,MPI_COMM_WORLD);
            }else 
            {
                bufi = 0;
                MPI_Send(&bufi,1,MPI_INT,SOURCE,99,MPI_COMM_WORLD);
            }


    	 }else if(funcion == 5){
            //QUIT
            int bufi;
            //Espero a recibir un quit
            
            // Libero mis recursos. El hashmap es local , se deberia destruir solo
            bufi = rank;
            // Le aviso al source que termine mandandole mi rank
            trabajarArduamente();
            MPI_Send(&bufi,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);
            
            break;

         }else if(funcion == 4){
            //MAXIMUM.
            
            //Elimino el mensaje de maximum recibido.
            char buf[1];
            MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);

            //Itero sobre mi hashMap mandandole palabra por palabra

            HashMap::iterator it = miHashMap.begin();
            HashMap::iterator itEnd = miHashMap.end();

            for (;it != itEnd; it++)
            {
                char palabra[(*it).size() + 1];
                strcpy(palabra, (*it).c_str());
                MPI_Send(palabra,(*it).size()+1, MPI_CHAR, SOURCE, 99 , MPI_COMM_WORLD);
            
            }
            
            //Le mando un mensaje a la consola que termine.
            char termine;
            trabajarArduamente();
                
            MPI_Send(&termine,1, MPI_CHAR,SOURCE, 90 , MPI_COMM_WORLD);
                    
         }

   }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
