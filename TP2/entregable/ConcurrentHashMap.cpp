#include "ConcurrentHashMap.hpp"

using namespace std;

      ConcurrentHashMap::ConcurrentHashMap(){
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
            int ik = hash(key[0]);
            writeLock(ik);
            Lista< pair<string, unsigned int> >::Iterador it = tabla[ik]->CrearIt();
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

         int numeroLista = 0;
         pthread_mutex_t mutex;
         pthread_mutex_init(&mutex, NULL);

         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
         for (i = 0; i < nt; i++)
         {
            threadParams[i]->context = this; 
            threadParams[i]->mutex = &mutex;
            threadParams[i]->numeroLista = &numeroLista;
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
      
   ConcurrentHashMap& ConcurrentHashMap::count_words(string arch){
    
    ConcurrentHashMap* res = new ConcurrentHashMap();
    //fijarse si al asignar por copia no hay shenanigans con la inicializacion de los mutex y cond variables
    ifstream file;
      file.open (arch);
      if (!file.is_open()) ;

      string word;
      while (file >> word)
      {
          res->addAndInc(word);
      }
      return *res;
  }

   ConcurrentHashMap& ConcurrentHashMap::count_words(unsigned int n, list<string>  archs){
      ConcurrentHashMap* res = new ConcurrentHashMap();
      int cantArchivos = archs.size();
      
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
        threadParams[i]->hashMap = res;
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
      

      return *res;
  }

  ConcurrentHashMap& ConcurrentHashMap::count_words(list<string>& archs){
     
      int cantArchivos = archs.size();
    
      return count_words(cantArchivos, archs);
   }

   pair<string, unsigned int> ConcurrentHashMap::maximum(unsigned int p_archivos, unsigned int p_maximos, list<string> archs){
        int cantArchivos = archs.size();

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

   pair<string, unsigned int> ConcurrentHashMap::maximum(list<string> archs){
      ConcurrentHashMap res;
      res = res.count_words(archs);
      return res.maximum(26);
   }
