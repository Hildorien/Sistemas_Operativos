#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <ctype.h>
#include <utility>
#include "consola.hpp"
#include "HashMap.hpp"
#include "mpi.h"

using namespace std;

#define CMD_LOAD    "load"
#define CMD_ADD     "addAndInc"
#define CMD_MEMBER  "member"
#define CMD_MAXIMUM "maximum"
#define CMD_QUIT    "quit"
#define CMD_SQUIT   "q"

static unsigned int np;
/*		  Tag = 1 ==> load()
		  Tag = 2 ==> addAndInc()
		  Tag = 3 ==> member()
		  Tag = 4 ==> Maximum()
		  Tag = 5 ==> Quit()
		  Tag = 90 ==> Respuesta de Nodo: Termino maximum
		  Tag = 99 ==> Respuesta de Nodo

*/

// Crea un ConcurrentHashMap distribuido
static void load(list<string> params) {
    
    queue<int> nodosIdle;
    MPI_Status status;
    
    //Inicializamos la cola
    for (unsigned int i = 1; i < np ; i++) //np nodos  + 1 nodo consola 
    {
    	nodosIdle.push(i);
    }

   	int checkout;
    
    for (list<string>::iterator it=params.begin(); it != params.end(); ++it) {
    	
    	if(nodosIdle.empty()){ //Si no hay nodos idle, esperamos a que haya.
    		//Como no sabemos que nodo se va a desocupar, hacemos un Probe que se fija su idRank
    		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    		
    		int nodoLibre = status.MPI_SOURCE; 
    		
    		MPI_Recv(&checkout,1,MPI_INT,nodoLibre,99,MPI_COMM_WORLD,&status);
    		
    		nodosIdle.push(nodoLibre); //pusheo el nuevo nodo libre
    	}
    
        int nodoATrabajar = nodosIdle.front();
    
        nodosIdle.pop();

        char libro[(*it).size()+1];

        strcpy(libro, (*it).c_str());

        cout << libro << endl;

        MPI_Request req;
    
        MPI_Isend(libro, (*it).size()+1 , MPI_CHAR, nodoATrabajar, 1 , MPI_COMM_WORLD, &req);
        //El tamaño es (*it).size + 1, ya que tambien queremos agregar el byte null. (si el buffer es de tamaño .size() no lo manda)

    }

    //Ya envie todos los libros que me pasaron como parametro
    //Ahora me quedo esperando a que todos los nodos vuelvan. O sea, a que la cola se llene de nuevo.
    while(nodosIdle.size() != np-1){
    	
    	MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    		
    	int nodoTermino = status.MPI_SOURCE; 
    		
    	MPI_Recv(&checkout,1,MPI_INT,nodoTermino,99,MPI_COMM_WORLD,&status);

    	nodosIdle.push(nodoTermino);

    }

    
    cout << "La listá esta procesada" << endl;
}

// Esta función debe avisar a todos los nodos que deben terminar
static void quit() {
    
    char checkout[4];
    int bufi[2];
     
    MPI_Status status;
    MPI_Request req;

    for (unsigned int i = 1; i < np ; i++)
    {
        //Le envio a todos un aviso de que tienen que hacer quit.
        MPI_Isend(checkout, 1 ,MPI_CHAR,i,5,MPI_COMM_WORLD, &req);
    }

    //Espero a quue todos los nodos me avisen que liberaron sus recursos
    for (unsigned int i = 1 ; i < np ; i++)
    {
    
        MPI_Recv(bufi,1,MPI_INT,MPI_ANY_SOURCE,99,MPI_COMM_WORLD,&status);
    }

}

// Esta función calcula el máximo con todos los nodos
static void maximum() {
    
    pair <string, unsigned int> result ("", 0);

    HashMap totalHashMap;
    MPI_Status status;
    char dummy = 'e';
    
    int msgTag;
    MPI_Request req;

    for (unsigned int i = 1; i < np; i++)
    {
    	//Envien sus palabras!
        MPI_Isend(&dummy,1,MPI_CHAR,i,4,MPI_COMM_WORLD, &req);
    }

    unsigned int contadorTerminados = 0;
    while (contadorTerminados < np - 1)
    {
        
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG , MPI_COMM_WORLD, &status);
        int largoMsj;
        
        MPI_Get_count(&status, MPI_CHAR, &largoMsj);
        //cout << "1er Probe: long = " << largoMsj << ", nodo = " << status.MPI_SOURCE;

        char palabra[largoMsj];
        msgTag = status.MPI_TAG;
        int source = status.MPI_SOURCE;

        if(msgTag == 99){

            //cout << " 2do Probe: long = " << largoMsj << ", nodo = " << status.MPI_SOURCE << endl;
            MPI_Recv(palabra, largoMsj, MPI_CHAR,source,99,MPI_COMM_WORLD,&status);
            
            totalHashMap.addAndInc(palabra);

            
        }

        if (msgTag == 90){
            
            //Tengo que asegurarme de descartar el mensaje
            MPI_Request req;
            MPI_Irecv(palabra, largoMsj, MPI_CHAR,source,90,MPI_COMM_WORLD, &req);
            
            contadorTerminados++;
           
        }
        
            
    }

    // Hacemos maximum con el hashmap de la consola.
    result = totalHashMap.maximum();
    
    cout << "El máximo es <" << result.first <<"," << result.second << ">" << endl;
  }


// Esta función busca la existencia de *key* en algún nodo
static void member(string key) {
    
    MPI_Status status;
    bool esta = false;
    //int tamMsj;
    char palabra[key.size()+1]; 
    int bufi; 

    strcpy(palabra, key.c_str()); //Mandamos la palabra con key
    for (unsigned int i = 1; i < np ; i++) //np nodos  + 1 nodo consola 
    {
        //Le envio a todos los nodos la palabra
        MPI_Request req;
        MPI_Isend(palabra, key.size()+1 ,MPI_CHAR,i,3,MPI_COMM_WORLD, &req);
    }

    for (unsigned int i = 1 ; i < np ; i++)
    {
    
        MPI_Recv(&bufi,1,MPI_INT,MPI_ANY_SOURCE,99,MPI_COMM_WORLD,&status);
        if( bufi == 1 ) 
        {
            esta = true;
        }

    
    }

    cout << "El string <" << key << (esta ? ">" : "> no") << " está" << endl;
}


// Esta función suma uno a *key* en algún nodo
static void addAndInc(string key) {

    MPI_Status status;
    int bufi; 
    char palabra[key.size()]; 
        

    MPI_Request alertReq[np-1];
   // cout << "Voy a a mandarle a todos un aviso" << endl;   
    for (unsigned int i = 1; i < np ; i++) //np nodos  + 1 nodo consola 
    {
        //Le envio a todos los nodos un aviso 
        MPI_Isend(palabra, key.size() ,MPI_CHAR,i,2,MPI_COMM_WORLD, &alertReq[i-1]);
    }

   //Obtengo el nodo que va a realizar el addandinc
   unsigned int nodoaLaburar;
    // Recibo ahora el permiso del nodo.
   // cout << "El nodo que va a laburar es " << nodoaLaburar << endl;
   // MPI_Recv(palabra,tamMsj,MPI_CHAR,nodoaLaburar,99,MPI_COMM_WORLD,&status);
   // cout << "Recibi de " << nodoaLaburar << "el permiso" << endl;

    MPI_Request recvReq[np-1];
    //Se crea un arreglo de Requests por cada nodo al que se le mande el mensaje.
    //Cada request es asociado a una peticion MPI_Irecv. Cuando efectivamente recibe un mensaje, 
    //su request correspondiente cambia de estado.
    for (unsigned int i = 1; i < np; i++)
    {
        MPI_Irecv(&bufi,1,MPI_INT,MPI_ANY_SOURCE,99,MPI_COMM_WORLD, &recvReq[i-1]);
    }
    int index;
    MPI_Waitany(np-1, recvReq, &index, &status);
    //Pasando como parametro el arreglo de requests, MPI_Waitany se bloquea hasta que uno de los requests tenga estado "Terminado". 
    //Dicho request es copiado a status.

    nodoaLaburar = status.MPI_SOURCE;

    MPI_Request addReq[np-1];
    for (unsigned int i = 1; i < np ; i++) //Para todos los nodos 
    {
        if( i == nodoaLaburar) //Si es el nodo que tiene que hacer addandInc
        {
           // cout << "Es el nodoaLaburar mandemosle en bufi un 1" << endl;
            bufi = 1; //Mandamos a bufi un 1
            // cout << "bufi tiene " << (*bufi) << " y la palabra a mandar es " << key << endl;
            MPI_Isend(&bufi,1,MPI_INT,i,2,MPI_COMM_WORLD, &addReq[i-1]);
        }
        else
        {   //Si no tiene que laburar enviamos en bufi un 0
            bufi = 0;
           // cout << "A " << i << " no le toca laburar , le mando un 0 en bufi " << endl;
            MPI_Isend(&bufi,1,MPI_INT,i,2,MPI_COMM_WORLD, &addReq[i-1]);
        }
    
     }
     //Enviamos por llamada no bloqueante, las confirmaciones a los nodos, sea para confirmar que tienen que trabajar o no.
     //Antes de avanzar, nos aseguramos que el nodo que tiene que trabajar, haya recibido el mensaje anterior.
    MPI_Wait(&addReq[nodoaLaburar-1],MPI_STATUS_IGNORE);
    strcpy(palabra, key.c_str()); //Mandamos la palabra con key

    //Una vez sabemos que el nodo esta listo para trabajar, le enviamos la palabra
    MPI_Request req;
    MPI_Isend(palabra, key.size() ,MPI_CHAR,nodoaLaburar,2,MPI_COMM_WORLD, &req);  


    for(unsigned int i = 1 ; i < np ; i++){       
        
        MPI_Recv(&bufi,1,MPI_INT,MPI_ANY_SOURCE,99,MPI_COMM_WORLD,&status);   
       
        //int nodoTermino = status.MPI_SOURCE; 
       
        //cout << "Recibi un aviso de que " << nodoTermino << " termino" << endl;
        
    }
    
    cout << "Agregado: " << key << endl;
}


/* static int procesar_comandos()
La función toma comandos por consola e invoca a las funciones correspondientes
Si devuelve true, significa que el proceso consola debe terminar
Si devuelve false, significa que debe seguir recibiendo un nuevo comando
*/

static bool procesar_comandos() {

    char buffer[BUFFER_SIZE];
    size_t buffer_length;
    char *res, *first_param, *second_param;

    // Mi mamá no me deja usar gets :(
    res = fgets(buffer, sizeof(buffer), stdin);

    // Permitimos salir con EOF
    if (res==NULL)
        return true;

    buffer_length = strlen(buffer);
    // Si es un ENTER, continuamos
    if (buffer_length<=1)
        return false;

    // Sacamos último carácter
    buffer[buffer_length-1] = '\0';

    // Obtenemos el primer parámetro
    first_param = strtok(buffer, " ");

    if (strncmp(first_param, CMD_QUIT, sizeof(CMD_QUIT))==0 ||
        strncmp(first_param, CMD_SQUIT, sizeof(CMD_SQUIT))==0) {

        quit();
        return true;
    }

    if (strncmp(first_param, CMD_MAXIMUM, sizeof(CMD_MAXIMUM))==0) {
        maximum();
        return false;
    }

    // Obtenemos el segundo parámetro
    second_param = strtok(NULL, " ");
    if (strncmp(first_param, CMD_MEMBER, sizeof(CMD_MEMBER))==0) {
        if (second_param != NULL) {
            string s(second_param);
            member(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_ADD, sizeof(CMD_ADD))==0) {
        if (second_param != NULL) {
            string s(second_param);
            addAndInc(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_LOAD, sizeof(CMD_LOAD))==0) {
        list<string> params;
        while (second_param != NULL)
        {
            string s(second_param);
            params.push_back(s);
            second_param = strtok(NULL, " ");
        }

        load(params);
        return false;
    }

    printf("Comando no reconocido");
    return false;
}

void consola(unsigned int np_param) {
    np = np_param;
    printf("Comandos disponibles:\n");
    printf("  "CMD_LOAD" <arch_1> <arch_2> ... <arch_n>\n");
    printf("  "CMD_ADD" <string>\n");
    printf("  "CMD_MEMBER" <string>\n");
    printf("  "CMD_MAXIMUM"\n");
    printf("  "CMD_SQUIT"|"CMD_QUIT"\n");

    bool fin = false;
    while (!fin) {
        printf("> ");
        fflush(stdout);
        fin = procesar_comandos();
    }
}
