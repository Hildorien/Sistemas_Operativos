#include "ConcurrentHashMap.hpp"

using namespace std;

      ConcurrentHashMap::ConcurrentHashMap(){
            tabla.resize(TABLE_SIZE); // El hash va a tener size 26
            for (int i = 0; i < TABLE_SIZE; i++){
               pthread_mutex_init(&mutexLock[i], NULL);
               pthread_cond_init(&condVarLock[i], NULL);
               writer[i] = false;
               currentReaders[i] = 0;
               tabla.push_back( new Lista<pair<string, unsigned int> >() );
            }
      }

      ConcurrentHashMap::~ConcurrentHashMap(){
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
           delete(tabla[i]);
           pthread_mutex_destroy(&mutexLock[i]);
           pthread_cond_destroy(&condVarLock[i]);
        }
      }

      int ConcurrentHashMap::hash(char a){
            int ia = ((int)a % 26); // No necesariamente a = 0; b = 1...
            return ia;
      }

      void ConcurrentHashMap::addAndInc(string key){
            cout << "Entre a addandInc " << endl;
            int ik = hash(key[0]);
            cout << "realize el hash y su valor es "  << ik << endl;
            writeLock(ik);
            cout << "puse un lock al hash" << endl;
            Lista< pair<string, unsigned int> >::Iterador it = tabla[ik]->CrearIt();
            cout << "cree el iterador" << endl;
            bool encontrado = false;
            cout << "Inicialize cosas " << endl;
            while(it.HaySiguiente() && !encontrado){
                  if(it.Siguiente().first == key){ 
                        it.Siguiente().second++;
                        encontrado = true;
                  }
            it.Avanzar();
            }
            cout << "Sali del ciclo y encontrado es" << encontrado << endl;
            if(encontrado == false){
                  pair<string, int>  entry(key, 1);
                  tabla[ik]->push_front(entry);      
             }
             writeUnlock(ik);
          
      }

      bool ConcurrentHashMap::member(string key){
            int ik = hash(key[0]);
            readLock(ik);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[ik]->CrearIt();
            while(it.HaySiguiente()){
                  if(it.Siguiente().first == key){ 
                  return true;  
                  }
            it.Avanzar();
            }
            readUnlock(ik);
            return false;

      }

      pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int nt){
             
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
         pthread_attr_destroy(&attr);

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
      
   ConcurrentHashMap ConcurrentHashMap::count_words(string arch){
    
    cout << "Entro a la funcion " << endl;
    ConcurrentHashMap res = ConcurrentHashMap();
    //fijarse si al asignar por copia no hay shenanigans con la inicializacion de los mutex y cond variables
    cout << "Creo un hashmap " << endl;
    ifstream file;
      file.open (arch);
      if (!file.is_open()) return ConcurrentHashMap();

      string word;
      cout << "va a ejecutar addandInc " << endl;
      while (file >> word)
      {
          res.addAndInc(word);
      }
      cout << "Realizo el addandInc " << endl;
      return res;
  }

   ConcurrentHashMap ConcurrentHashMap::count_words(unsigned int n, Lista<string>&  archs){
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
      }

      for (int tid = 0; tid < n; ++tid){
               pthread_join(thread[tid], NULL);
      }
      pthread_attr_destroy(&attr);
         
      return res;
  }

  ConcurrentHashMap ConcurrentHashMap::count_words(Lista<string>& archs){
     
      int cantArchivos = 0;
      for (Lista<string>::Iterador it = archs.CrearIt(); it.HaySiguiente(); it.Avanzar())
      {
        cantArchivos++;
      }
      return count_words(cantArchivos, archs);
   }

   pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int p_archivos, unsigned int p_maximos, Lista<string> archs){
        int cantArchivos = 0;
        for (Lista<string>::Iterador it = archs.CrearIt(); it.HaySiguiente(); it.Avanzar()) cantArchivos++;
        ConcurrentHashMap* hashArray[cantArchivos];
        for (int i = 0; i < cantArchivos; ++i)
        {
          hashArray[i] = new ConcurrentHashMap();
        }
        pthread_t thread[p_archivos];

        vector<count_wordParams*> threadParams;
        for (int i = 0; i < p_archivos; ++i)
        {
          threadParams.push_back(new count_wordParams());
        }
      
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      
        for (int i = 0; i < p_archivos; i++)
        { 
          threadParams[i]->hashMaps = hashArray;
          threadParams[i]->index = i;
          threadParams[i]->cantArchivos = cantArchivos;
          threadParams[i]->numberThreads = p_archivos;
          threadParams[i]->archs = &archs;
          pthread_create(&thread[i], &attr, agregarArchivoMaximum, (void *)threadParams[i]); //mandar como parametros concurrenthashmap POR REFERENCIA, y el pathname
        }

        for (int tid = 0; tid < p_archivos; ++tid){
               pthread_join(thread[tid], NULL);
        }
        
        for (int i = 0; i < p_maximos; i++)
        { 
          threadParams[i]->hashMaps = hashArray;
          threadParams[i]->index = i;
          threadParams[i]->cantArchivos = cantArchivos;
          threadParams[i]->numberThreads = p_maximos;
          pthread_create(&thread[i], &attr, mergearCHashMap, (void *)threadParams[i]); //mandar como parametros concurrenthashmap POR REFERENCIA, y el pathname
        }

        for (int tid = 0; tid < p_maximos; ++tid){
               pthread_join(thread[tid], NULL);
        }
        
        pthread_attr_destroy(&attr);
         

        return (*hashArray)->maximum(p_maximos); //devuelvo el maximo del primer hashmap
   }

   pair<string, unsigned int> ConcurrentHashMap::maximum(Lista<string> archs){
      ConcurrentHashMap res;
      res = res.count_words(archs);
      return res.maximum(26);
   }

 /*  int main() {
    cout << "Hola" << endl;
    return 0;
   }*/