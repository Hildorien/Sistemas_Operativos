#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
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
    //int count = 4;
    //MPI_CHAR* buf
    char* buf;
    int* bufi;
    //cout << "Cree cosas del nodo" << endl;
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
        buf = (char* )malloc(msjcount);
    	

    	//Leo el tag e identifico que funcion tengo que correr.
    	int funcion = status.MPI_TAG;

    	/*funcion = 1 ==> load()
		  funcion = 2 ==> addAndInc()
		  funcion = 3 ==> member()
		  funcion = 4 ==> Maximum()
		  funcion = 5 ==> Quit()
		*/
    	//cout << "ARRANQUE, SOY  " << rank << endl;
    	if (funcion == 1){
    		/* LOAD */
    		//Recepcion bloqueante

    		MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);

    		//cout << "Me quede esperando a recibir de la consola y ya esta" << endl;
    		//Estamos en condiciones de empezar LOAD. En buff tenemos los parametros. 
            buf[msjcount] = 0;
    		//cout << "A punto de agregar el archivo " << buf << " con " << msjcount << " de tamaño" <<  endl;
            miHashMap.load(buf);
    		trabajarArduamente();
    		//cout << "hice load y TRABAJO TRABAJO!" << endl;
    		free(buf);

    		int* soyRank;

    		soyRank = (int* )malloc(sizeof(MPI_INT));
    		soyRank[0] = rank;
    		//cout << "pedi malloc para mirank" << endl;
    		//Respuesta OK bloqueante.
    		//Se lee: Pasar el contenido que puse en &SOYRANK, 1 elemento del tipo, MPI_INT, destinado a SOURCE, con el tag 99, y furta_comm_world
    		//cout << "TERMINE, SOY  " << rank << endl;
    		MPI_Send(soyRank,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);
    		//cout << "Yo hago un send a la consola manndandole un int" << endl;
    		free(soyRank);
    		
    	} else if (funcion == 2){ 
    		/*ADD AND INC*/
 
            bufi = (int* )malloc(4); //Malloc size 4, tamaño de un entero.
    		int tamMsj;
    		//Primer Recv. Los nodos se enteran que hay para hacer un addAndInc
    		MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);
			//cout << "Recibi un aviso de correr addAndInc" << endl;
			//Le avisamos a SOURCE que esuchamos la orden mandandole nuestro rank
			trabajarArduamente();
			//cout << ""
            MPI_Send(bufi,1,MPI_INT,SOURCE,99,MPI_COMM_WORLD);

		
    		//cout << "Le mande a consola que lo voy a correr" << endl;
			//Me quedo esperando a la confirmacion de source (si tengo que hacer addAndInc o no)
			MPI_Recv(bufi,1,MPI_INT,SOURCE,funcion,MPI_COMM_WORLD,&status);
            //cout << "Recibi la confirmacion de la consola, me toca correr?" << endl;
			if ( (*bufi) == 1)
			{
				
                //cout << "Soy " << rank << " y me toca hacer addAndInc" << endl;
                //Recibi un 1. Entonces me toca trabajar. Me quedo esperando a que me mande el string
				MPI_Probe(SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Get_count(&status, MPI_CHAR, &tamMsj);

				free(buf);
				buf = (char* )malloc(tamMsj);

				MPI_Recv(buf,tamMsj,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);
				buf[tamMsj] = 0;
                //cout << "A punto de agrgear POR ADDANDINC: " << buf << endl;
				miHashMap.addAndInc(buf);
                //cout << "ejecute addAndInc con el buf que tiene la key" << endl;
				trabajarArduamente();
               
			}

			 int* soyRank;
             soyRank = (int* )malloc(sizeof(MPI_INT));
             soyRank[0] = rank;
                //cout << "pedi malloc para mirank" << endl;
                //Respuesta OK bloqueante.
                //Se lee: Pasar el contenido que puse en &SOYRANK, 1 elemento del tipo, MPI_INT, destinado a SOURCE, con el tag 99, y furta_comm_world
            //cout << "TERMINE, SOY  " << rank << endl;
            MPI_Send(soyRank,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);

                //cout << "Yo hago un send a la consola manndandole un int" << endl;
            free(soyRank);
			free(bufi);
			free(buf);

    	 }else if ( funcion == 3) {
            //Member

            bufi = (int* )malloc(4); //Malloc size 4, tamaño de un entero.

            //Primer recieved. Los nodos se enteran de que tienen que hacer member
            MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);

            buf[msjcount] = 0;
           // cout << "me preguntaron si esta: " << buf << endl;
            bool res = miHashMap.member(buf);
            trabajarArduamente();
            //cout << "Y esta?: " << res << endl;
            if (res) 
            {
                (*bufi) = 1;
                MPI_Send(bufi,1,MPI_INT,SOURCE,99,MPI_COMM_WORLD);
            }else 
            {
                (*bufi) = 0;
                MPI_Send(bufi,1,MPI_INT,SOURCE,99,MPI_COMM_WORLD);
            }

            free(bufi);
            free(buf);

    	 }else if(funcion == 5){
            //QUIT
            bufi = (int* )malloc(4); //Malloc size 4, tamaño de un entero.
            //Espero a recibir un quit
           // MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);

            // Libero mis recursos. El hashmap es local , se deberia destruir solo
            (*bufi) = rank;
            // Le aviso al source que termine mandandole mi rank
            trabajarArduamente();
            MPI_Send(bufi,1, MPI_INT,SOURCE, 99 , MPI_COMM_WORLD);
            
            free(bufi);
            free(buf);
            break;

         }else if(funcion == 4){
            //MAXIMUM.
            //Elimino el mensaje de maximum recibido.
            MPI_Recv(buf,msjcount,MPI_CHAR,SOURCE,funcion,MPI_COMM_WORLD,&status);


            //Itero sobre mi hashMap mandandole palabra por palabra

            HashMap::iterator it = miHashMap.begin();
            HashMap::iterator itEnd = miHashMap.end();

            for (;it != itEnd; it++)
            {
            	buf = (char*)malloc((*it).size());
                
                strcpy(buf, (*it).c_str());
                
                trabajarArduamente();
                //cout << "Mando la palabra: " << buf << endl;
                MPI_Send(buf,(*it).size(), MPI_CHAR, SOURCE, 99 , MPI_COMM_WORLD);
            
            }
            /*
            for (list<pair<string, unsigned int> >::iterator it = miHashMap.begin(); it != miHashMap.end(); ++it){
                buf = (char*)malloc((*it).first.size());
                
                strcpy(buf, (*it).first.c_str());
                
                trabajarArduamente();
                MPI_Send(buf,(*it).first.size(), MPI_CHAR, SOURCE, 99 , MPI_COMM_WORLD);
            }
*/
            //Le mando un mensaje a la consola que termine.
                char* termine;
                termine = (char* )malloc(sizeof(MPI_CHAR));
                termine[0] = 't';                
                trabajarArduamente();
                //cout << "Soy el nodo " << rank << "Y ya termine! me puedo morir?" << endl;
                MPI_Send(termine,1, MPI_CHAR,SOURCE, 90 , MPI_COMM_WORLD);
               // cout << "Soy el nodo " << rank << "Y ya me confirmaron que me puedo morir!" << endl;             
                free(termine);
                free(buf);
         }

   }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
