#include <vector>;
#include <string>;
#include <pair>;
#include <iostream>;
#include "ListaAtomica.hpp"
using namespace std;



const int TABLE_SIZE = 26;

class ConcurrentHashMap {
private:
       Lista< pair<string, unsigned int> > [TABLE_SIZE] tabla ;
       int[TABLE_SIZE] filasLockeadas;

public:
      ConcurrentHashMap() {
            tabla = new Lista<pair<string, unsigned int>> [TABLE_SIZE];
            for (int i = 0; i < TABLE_SIZE; i++){
                  tabla[i] = Lista();
                  filasLockeadas[i] = 1;
            }
      }
      

      int hash(char a){
            int ia = ((int)a % 26);
            return ia;
      }

      void addAndInc(string key){
            int ik = hash(key[0]);
            while (filasLockeadas[ik] != 1){ /*DORMIR, WAIT, LO QUE SEA*/}
            filasLockeadas[ik] -= 1;

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

            filasLockeadas[ik]++;
            
      }
      bool member(string key){
            
      }

      pair<string, unsigned int> maximum(unsigned int nt){}
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