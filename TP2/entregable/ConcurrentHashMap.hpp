#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <list>
#include "ListaAtomica.hpp"

const int TABLE_SIZE = 26;
using namespace std;

class ConcurrentHashMap {
private:

    struct count_wordParams{
      list<string>* archs;
      int index;
      int numberThreads;
      int cantArchivos;
      ConcurrentHashMap* hashMap;
      ConcurrentHashMap** hashMaps;
      };
      struct maximumParams{
         ConcurrentHashMap *context;
         pthread_mutex_t* mutex;
         int* numeroLista;
         pair<string, unsigned int> solution; 
      };

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
         pthread_mutex_unlock(&mutexLock[i]);
       }

       void *getMaximumFromRows(void *params){
         maximumParams *paramsPuntero = (maximumParams*) params;
         pair<string, unsigned int> maximo;
         maximo.second = 0;

         pthread_mutex_lock(paramsPuntero->mutex);
         while (*(paramsPuntero->numeroLista) < 26) {
            int filaALeer = *(paramsPuntero->numeroLista);
           *(paramsPuntero->numeroLista) = filaALeer + 1;
            pthread_mutex_unlock(paramsPuntero->mutex);

            readLock(filaALeer);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[filaALeer]->CrearIt();
            while(it.HaySiguiente()){
                  if(it.Siguiente().second > maximo.second){ 
                   maximo = it.Siguiente();
                  }
            it.Avanzar();
            }
            readUnlock(filaALeer);

            pthread_mutex_lock(paramsPuntero->mutex);
         }
         pthread_mutex_unlock(paramsPuntero->mutex);
         
         paramsPuntero->solution = maximo;
       }

       static void *getMaximumFromRowsHelper(void *params){
         maximumParams *paramsPuntero = (maximumParams*) params;
         paramsPuntero->context->getMaximumFromRows(params);
         pthread_exit(NULL);
       }

        

        void copiarCHashMap(ConcurrentHashMap* copy){
          for (int i = 0; i < TABLE_SIZE; ++i)
          {
            Lista<pair<string, unsigned int> >::Iterador it = tabla[i]->CrearIt();
            for (it; it.HaySiguiente(); it.Avanzar())
            {   
              for (int j = 0; j < it.Siguiente().second; ++j)
              {
                copy->addAndInc(it.Siguiente().first);
              }
                
            }
          }
          
        }

        static void *mergearCHashMap(void *params){
          count_wordParams *paramsPuntero = (count_wordParams*) params;
          int index = paramsPuntero->index;
          int numberThreads = paramsPuntero->numberThreads;
          int cantHashMaps = paramsPuntero->cantArchivos;
          ConcurrentHashMap** hashMaps = paramsPuntero->hashMaps; 
          int i = index;
          while(i < cantHashMaps)
          {
              if (i != 0)
              {
                hashMaps[i]->copiarCHashMap(hashMaps[0]);
              }
              i += numberThreads;
          } 
          pthread_exit(NULL);
        }


         static void *agregarArchivoCHM(void *params){
        //casteo a puntero a parametros el argumento void
        count_wordParams *paramsPuntero = (count_wordParams*) params;
            //creo un iterador a la lista atomica de archivos, como se que nadie la va a estar modificando no hay problema que varios threads lean
        list<string>::iterator it = paramsPuntero->archs->begin();
        //posiciono el iterador en el primer archivo a leer (index)
        for (int i = 0; i < paramsPuntero->index; i++)
        {
            it++;
        }
        //por cada archivo que el thread debe procesar hago lo siguiente (archivos = todo k < cantArchivos tal que k % numberThreads = k)
        //cout << paramsPuntero->index << " " << paramsPuntero->cantArchivos << " " << paramsPuntero->numberThreads << endl;
        int i = paramsPuntero->index;

        while(i < paramsPuntero->cantArchivos)
        {
          ifstream file;
          file.open (*it);
           if (!file.is_open()) cout << *it << endl;
  
           string word;
           while (file >> word)
           {
               paramsPuntero->hashMap->addAndInc(word);
           } 
           //avanzo el iterador de archivos hasta el siguiente a procesar o que se termine la lista, en cuyo caso el proximo for va a hacer break
           //cout << "Prox Arch" << endl;
           for (int j = 0; j < paramsPuntero->numberThreads && it != paramsPuntero->archs->end(); j++)
           {
               it++;
           }
           //cout << "Tengo PA" << endl;
          file.close();
          i += paramsPuntero->numberThreads;
          
         }

         pthread_exit(NULL);
        
      }

      static void *agregarArchivoMaximum(void *params){
         count_wordParams *paramsPuntero = (count_wordParams*) params;
         list<string>* archs = paramsPuntero->archs;
         int index = paramsPuntero->index;
         int numberThreads = paramsPuntero->numberThreads;
         int cantArchivos = paramsPuntero->cantArchivos;
         ConcurrentHashMap** hashMaps = paramsPuntero->hashMaps;
         int i = index;
        while( i < cantArchivos)
        {
           int j = 0;
           for (list<string>::iterator it = archs->begin(); it != archs->end(); it++)
           {
            if (i==j) {
              *hashMaps[i] = hashMaps[i]->count_words(*it);
              break;
            }
            j++;
           }
          i += numberThreads; 
         }
         pthread_exit(NULL);

        }


public:

  vector<  Lista< pair<string, unsigned int> >* > tabla;
  ConcurrentHashMap();
  ~ConcurrentHashMap();
  int hash(char a);
  void addAndInc(string key);
  bool member(string key);
  pair<string, unsigned int> maximum(unsigned int nt); 
  static ConcurrentHashMap& count_words(string arch);
  static ConcurrentHashMap& count_words(unsigned int n, list<string> archs);
  static ConcurrentHashMap& count_words(list<string>& archs);
  static pair<string, unsigned int> maximum(unsigned int p_archivos, unsigned int p_maximos, list<string> archs); 
  static pair<string, unsigned int> maximum(list<string> archs);
      
};
