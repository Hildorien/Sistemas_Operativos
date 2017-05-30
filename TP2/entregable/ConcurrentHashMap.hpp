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
      pthread_mutex_t* mutex;
      int* numeroArchivo;
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
          ConcurrentHashMap** hashMaps = paramsPuntero->hashMaps; 
          int cantHashMaps = paramsPuntero->cantArchivos;
          
          pthread_mutex_t* mutex = paramsPuntero->mutex;
          int* numeroHashmap = paramsPuntero->numeroArchivo;

          pthread_mutex_lock(mutex);
          while(*numeroHashmap < cantHashMaps)
          {    
             int hashMapProcesar = *numeroHashmap;
             *numeroHashmap = hashMapProcesar + 1;
             pthread_mutex_unlock(mutex);

             hashMaps[hashMapProcesar]->copiarCHashMap(hashMaps[0]);
            pthread_mutex_lock(mutex);
          } 
          pthread_mutex_unlock(mutex);
          pthread_exit(NULL);
        }


         static void *agregarArchivoCHM(void *params){
        //casteo a puntero a parametros el argumento void
        count_wordParams *paramsPuntero = (count_wordParams*) params;
            //creo un iterador a la lista atomica de archivos, como se que nadie la va a estar modificando no hay problema que varios threads lean
        list<string>::iterator it = paramsPuntero->archs->begin();
        int ultimoArchivoProcesado = -1;
        //por cada archivo que el thread debe procesar hago lo siguiente (archivos = todo k < cantArchivos tal que k % numberThreads = k)
        //cout << paramsPuntero->index << " " << paramsPuntero->cantArchivos << " " << paramsPuntero->numberThreads << endl;
        pthread_mutex_lock(paramsPuntero->mutex);
        while(*(paramsPuntero->numeroArchivo) < paramsPuntero->archs->size())
        {
          int archivoProcesar = *(paramsPuntero->numeroArchivo);
          *(paramsPuntero->numeroArchivo) = archivoProcesar + 1;
          pthread_mutex_unlock(paramsPuntero->mutex);
        
          while(ultimoArchivoProcesado != -1 && ultimoArchivoProcesado<archivoProcesar)
          {
            ultimoArchivoProcesado++;
            it++;
          }

          ifstream file;
          file.open (*it);
           if (!file.is_open()) cout << *it << endl;
  
           string word;
           while (file >> word)
           {
               paramsPuntero->hashMap->addAndInc(word);
           } 
          
          file.close();  
          pthread_mutex_lock(paramsPuntero->mutex);        
         }
         pthread_mutex_unlock(paramsPuntero->mutex);
        

         pthread_exit(NULL);
        
      }

      static void *agregarArchivoMaximum(void *params){
         count_wordParams *paramsPuntero = (count_wordParams*) params;
         list<string>* archs = paramsPuntero->archs;
         ConcurrentHashMap** hashMaps = paramsPuntero->hashMaps;
         
         list<string>::iterator it = archs->begin();
         int* numeroArchivo = paramsPuntero->numeroArchivo;
         pthread_mutex_t* mutex = paramsPuntero->mutex;
         int ultimoArchivoProcesado = -1;

        pthread_mutex_lock(mutex); 
        while( *numeroArchivo < archs->size())
        {
          int archivoProcesar = *numeroArchivo;
          *numeroArchivo = archivoProcesar + 1;
          pthread_mutex_unlock(mutex);

          while(ultimoArchivoProcesado != -1 && ultimoArchivoProcesado<archivoProcesar)
          {
            ultimoArchivoProcesado++;
            it++;
          }
          
          *hashMaps[archivoProcesar] = hashMaps[archivoProcesar]->count_words(*it);
           pthread_mutex_lock(mutex); 
        
        }
        pthread_mutex_unlock(mutex);
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
