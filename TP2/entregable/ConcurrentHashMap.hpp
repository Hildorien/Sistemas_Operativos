#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include "ListaAtomica.hpp"

const int TABLE_SIZE = 26;
using namespace std;

class ConcurrentHashMap {
private:

      struct maximumParams{
         ConcurrentHashMap *context;
         int index;
         int numberThreads;
         pair<string, unsigned int> solution; 
      };

       vector<  Lista< pair<string, unsigned int> > > tabla; // Segun internet es mejor tener un vector para no tener que de-alocar el arrelgo con delete[].
       //Lista< pair<string, unsigned int> > tabla[TABLE_SIZE];
       pthread_mutex_t mutexLock[TABLE_SIZE];
       pthread_cond_t condVarLock[TABLE_SIZE];

       int currentReaders[TABLE_SIZE]; //Almacena el numero de maximums que estan en simultaneo leyendo una lista
       bool writer[TABLE_SIZE]; //Almacena un bool que dice si hay un thread escribiendo en la lista

       void readLock(int i){
         pthread_mutex_lock(&mutexLock[i]);
         while(writer[i])
            pthread_cond_wait(&condVarLock[i],&mutexLock[i]);
         currentReaders[i]++;
         pthread_mutex_unlock(&mutexLock[i]);
       }

       void readUnlock(int i){
         pthread_mutex_lock(&mutexLock[i]);
         currentReaders[i]--;
         if (currentReaders[i] == 0)
            pthread_cond_signal(&condVarLock[i]);
         pthread_mutex_unlock(&mutexLock[i]);
       }

       void writeLock(int i){
         pthread_mutex_lock(&mutexLock[i]);
         while(writer[i] || currentReaders[i] > 0)
            pthread_cond_wait(&condVarLock[i], &mutexLock[i]);
         writer[i] = true;
         pthread_mutex_unlock(&mutexLock[i]);
       }

       void writeUnlock(int i){
         pthread_mutex_lock(&mutexLock[i]);
         writer[i] = false;
         pthread_cond_signal(&condVarLock[i]);
         pthread_mutex_lock(&mutexLock[i]);
       }

       void *getMaximumFromRows(void *params){
         maximumParams *paramsPuntero = (maximumParams*) params;
         pair<string, unsigned int> maximo;
         maximo.second = 0;
         int j = paramsPuntero->index;
         for (j; j < TABLE_SIZE; j + paramsPuntero->numberThreads )
         {  
            readLock(j);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[j].CrearIt();
            while(it.HaySiguiente()){
                  if(it.Siguiente().second > maximo.second){ 
                   maximo = it.Siguiente();
                  }
            it.Avanzar();
            }
            readUnlock(j);
         }
         paramsPuntero->solution = maximo;
       }

       static void *getMaximumFromRowsHelper(void *params){
         maximumParams *paramsPuntero = (maximumParams*) params;
         paramsPuntero->context->getMaximumFromRows(params);
       }


public:

      ConcurrentHashMap(){
            tabla.resize(TABLE_SIZE); // El hash va a tener size 26
            for (int i = 0; i < TABLE_SIZE; i++){
               pthread_mutex_init(&mutexLock[i], NULL);
               pthread_cond_init(&condVarLock[i], NULL);
               writer[i] = false;
               currentReaders[i] = 0;
               Lista< pair<string, unsigned int> > vi;
               tabla.push_back(vi);
            }
      }

      ~ConcurrentHashMap(){}

      int hash(char a){
            int ia = ((int)a % 26); // No necesariamente a = 0; b = 1...
            return ia;
      }

      void addAndInc(string key){
            int ik = hash(key[0]);
            writeLock(ik);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[ik].CrearIt();
            bool encontrado = false;
            while(it.HaySiguiente() && !encontrado){
                  if(it.Siguiente().first == key){ 
                        it.Siguiente().second++;
                        encontrado = true;
                  }
            it.Avanzar();
            }
            
            if(encontrado == false){
                  pair<string, int>  entry(key, 1);
                  tabla[ik].push_front(entry);      
             }
             writeUnlock(ik);
            

      }
      bool member(string key){
            int ik = hash(key[0]);
            readLock(ik);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[ik].CrearIt();
            while(it.HaySiguiente()){
                  if(it.Siguiente().first == key){ 
                  return true;
                  }
            it.Avanzar();
            }
            readUnlock(ik);
            return false;

      }
      pair<string, unsigned int> maximum(unsigned int nt){
             
         pthread_t thread[nt]; int tid; int i;
         vector<maximumParams*> threadParams;
         for (int i = 0; i < nt; ++i)
         {
         	threadParams.push_back(new maximumParams());
         }
         //maximumParams* threadParams[nt]; //inicializar punteros a struct
         int j = nt;
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
         for (i = 0; i < nt; i++)
         {
            threadParams[i] = new maximumParams();
            threadParams[i]->context = this;
            threadParams[i]->index = i;
            threadParams[i]->numberThreads = nt; 
            pthread_create(&thread[i], &attr, getMaximumFromRowsHelper, (void *)threadParams[i]);
         }
         for (tid = 0; tid < nt; ++tid){
               pthread_join(thread[tid], NULL);
         }

         pair<string, unsigned int> res;
         res.second = 0;
         for (int i = 0; i < nt; i++)
         {
         	if (res.second < (threadParams[i]->solution).second )
         	{
         		res.second = (threadParams[i]->solution).second;
         		res.first = (threadParams[i]->solution).first;
         	}
         }

         return res;
      }
      
   
};
  
  struct count_wordParams{
    Lista<string>* archs;
    int index;
    int numberThreads;
    int cantArchivos;
    ConcurrentHashMap* hashMap;
  };

	ConcurrentHashMap count_words(string arch){
		ConcurrentHashMap res = ConcurrentHashMap();
		//fijarse si al asignar por copia no hay shenanigans con la inicializacion de los mutex y cond variables
		
		ifstream file;
    	file.open (arch);
    	if (!file.is_open()) return ConcurrentHashMap();

    	string word;
    	while (file >> word)
    	{
        	res.addAndInc(word);
    	}
			
    	return res;
	}

	void *agregarArchivoCHM(void *params){
    //casteo a puntero a parametros el argumento void
    count_wordParams *paramsPuntero = (count_wordParams*) params;
    //creo un iterador a la lista atomica de archivos, como se que nadie la va a estar modificando no hay problema que varios threads lean
    Lista<string>::Iterador it = paramsPuntero->archs->CrearIt();
    //posiciono el iterador en el primer archivo a leer (index)
    for (int i = 0; i < paramsPuntero->index; ++i)
    {
        it.Avanzar();
    }
    //por cada archivo que el thread debe procesar hago lo siguiente (archivos = todo k < cantArchivos tal que k % numberThreads = k)
    for (int i = paramsPuntero->index; i < paramsPuntero->cantArchivos; i + paramsPuntero->numberThreads)
    {
      ifstream file;
      file.open (it.Siguiente());
      if (!file.is_open()) ;

      string word;
      while (file >> word)
      {
          paramsPuntero->hashMap->addAndInc(word);
      } 
      //avanzo el iterador de archivos hasta el siguiente a procesar o que se termine la lista, en cuyo caso el proximo for va a hacer break
      for (int j = 0; it.HaySiguiente() && j < paramsPuntero->numberThreads; i++)
      {
          it.Avanzar();
      }

    }
    
  }

  ConcurrentHashMap count_words(Lista<string> archs){
	   	ConcurrentHashMap res = ConcurrentHashMap();
 	  	int cantArchivos = 0;
 	  	for (Lista<string>::Iterador it = archs.CrearIt(); it.HaySiguiente(); it.Avanzar())
 	  	{
 	  		cantArchivos++;
      }
      pthread_t thread[cantArchivos];

      vector<count_wordParams*> threadParams;
      for (int i = 0; i < cantArchivos; ++i)
      {
        threadParams.push_back(new count_wordParams());
      }
      
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      
   		for (int i = 0; i < cantArchivos; i++)
   		{	
        threadParams[i]->hashMap = &res;
        threadParams[i]->index = i;
        threadParams[i]->cantArchivos = cantArchivos;
        threadParams[i]->numberThreads = cantArchivos;
        threadParams[i]->archs = &archs;
   			pthread_create(&thread[i], &attr, agregarArchivoCHM, (void *)threadParams[i]); //mandar como parametros concurrenthashmap POR REFERENCIA, y el pathname
        i++;
      }
   		return res;
   }
  ConcurrentHashMap count_words(unsigned int n, Lista<string> archs){
      ConcurrentHashMap res = ConcurrentHashMap();
      int cantArchivos = 0;
      for (Lista<string>::Iterador it = archs.CrearIt(); it.HaySiguiente(); it.Avanzar())
      {
        cantArchivos++;
      }
      pthread_t thread[n];

      vector<count_wordParams*> threadParams;
      for (int i = 0; i < n; ++i)
      {
        threadParams.push_back(new count_wordParams());
      }
      
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      
      for (int i = 0; i < n; i++)
      { 
        threadParams[i]->hashMap = &res;
        threadParams[i]->index = i;
        threadParams[i]->cantArchivos = cantArchivos;
        threadParams[i]->numberThreads = n;
        threadParams[i]->archs = &archs;
        pthread_create(&thread[i], &attr, agregarArchivoCHM, (void *)threadParams[i]); //mandar como parametros concurrenthashmap POR REFERENCIA, y el pathname
        i++;
      }
      return res;
  }
      /*pair<string, unsigned int> maximum(unsigned int p archivos, unsigned int p maximos, Lista<string>archs){}
      */

 /*
   int get(int key) {
         int hash = (key % TABLE_SIZE);
         while (table[hash] != NULL && table[hash]->getKey() != key)
               hash = (hash + 1) % TABLE_SIZE;
         if (table[hash] == NULL)
               return -1;
         else
               return table[hash]->getValue();
   }

   void put(int key, int value) {
         int hash = (key % TABLE_SIZE);
         while (table[hash] != NULL && table[hash]->getKey() != key)
               hash = (hash + 1) % TABLE_SIZE;
         if (table[hash] != NULL)
               delete table[hash];
         table[hash] = new HashEntry(key, value);
   }     

   ~HashMap() {
         for (int i = 0; i < TABLE_SIZE; i++)
               if (table[i] != NULL)
                     delete table[i];
         delete[] table;
   }
   */