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
       mutex adiLockeados[TABLE_SIZE];
       mutex maxLockeados[TABLE_SIZE];

public:

      ConcurrentHashMap(){
            tabla.resize(TABLE_SIZE); // El hash va a tener size 26
            for (int i = 0; i < TABLE_SIZE; i++){
                 //tabla[i] = Lista();
            }
      }

      ~ConcurrentHashMap(){}

      int hash(char a){
            int ia = ((int)a % 26); // No necesariamente a = 0; b = 1...
            return ia;
      }

      void addAndInc(string key){
            int ik = hash(key[0]);
            maxLockeados[ik].try_lock(); //averiguo si el lock de max esta tomado, si no lo esta lo blockeo
            adiLockeados[ik].lock();//aca lockeamos el mutex que le corresponde a la key del string
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
            adiLockeados[ik].unlock(); //deslockeamos el mutex
        

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
             /*
         pthread_t thread[nt]; int tid; int i;
         int j = nt;
         i = 0;
         while( i < TABLE_SIZE && filasLockeadas[i] != 0  && j > 0){
               j--;
         }
         for (tid = 0; tid < nt; ++tid){
               pthread_join(thread[tid], NULL);
         }*/
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