#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include "ListaAtomica.hpp"

const int TABLE_SIZE = 26;
using namespace std;

class ConcurrentHashMap {
private:
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

public:

      ConcurrentHashMap(){
            tabla.resize(TABLE_SIZE); // El hash va a tener size 26
            for (int i = 0; i < TABLE_SIZE; i++){
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
            

      }
      bool member(string key){
            int ik = hash(key[0]);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[ik].CrearIt();
            while(it.HaySiguiente()){
                  if(it.Siguiente().first == key){ 
                  return true;
                  }
            it.Avanzar();
            }
            return false;

      }
      pair<string, unsigned int> maximum(unsigned int nt){
             
         pthread_t thread[nt]; int tid; int i;
         int j = nt;
         i = 0;
         while( i < TABLE_SIZE && j > 0){
               j--;
         }
         for (tid = 0; tid < nt; ++tid){
               pthread_join(thread[tid], NULL);
         }
      }
      /*
      CouncurrentHashMap count_words(string arch){}
      ConcurrentHashMap count words(Lista<string> archs){}
      ConcurrentHashMap count words(unsigned int n, Lista<string> archs){}
      pair<string, unsigned int> maximum(unsigned int p archivos, unsigned int p maximos, Lista<string>archs){}
      */
};


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