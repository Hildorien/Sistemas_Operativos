#include <vector>;
#include <string>;
#include <pair>;
#include <iostream>;
#include <mutex> 
#include "ListaAtomica.hpp"
using namespace std;



const int TABLE_SIZE = 26;

class ConcurrentHashMap {
private:
       Lista< pair<string, unsigned int> > [TABLE_SIZE] tabla ;
       mutex[TABLE_SIZE] adiLockeados;
       mutex[TABLE_SIZE] maxLockeados;

public:
      ConcurrentHashMap() {
            tabla = new Lista<pair<string, unsigned int>> [TABLE_SIZE];
            for (int i = 0; i < TABLE_SIZE; i++){
                  tabla[i] = Lista();
            }
      }
      

      int hash(char a){
            int ia = ((int)a % 26); // No necesariamente a = 0; b = 1...
            return ia;
      }

      void addAndInc(string key){
            int ik = hash(key[0]);
            maxLockeados[ik].try_lock(); //averiguo si el lock de max esta tomado, si no lo esta lo blockeo
            adiLockeados[ik].lock();//aca lockeamos el mutex que le corresponde a la key del string

            Iterador it = tabla[ik].CrearIt();
            bool encontrado = false;
            while(it.haySiguiente() && !encontrado){
                  if(it.Siguiente().first == key){ 
                        it.Siguiente().second++;
                        encontrado = true;
                  }
                  it.Avanzar();
            }

            if(encontrado == false;){
                  pair<string, int>  entry(key, 1);
                  tabla[ik].push_front(entry);      
            }

            adiLockeados[ik].unlock(); //deslockeamos el mutex
            
      }
      bool member(string key){
             int ik = hash(key[0]);
             Iterador it = tabla[ik].CrearIt();
              while(it.haySiguiente()){

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

            while( i < TABLE_SIZE && filasLockeadas[i] != 0  && j > 0){

                  j--;
            }

            for (tid = 0; tid < nt; ++tid){
                  pthread_join(thread[tid], NULL);
            }

      }
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
};