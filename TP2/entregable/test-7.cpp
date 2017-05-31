#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(void) {
	pair<string, unsigned int> p;
	list<string> l = { "corpus", "corpus", "corpus", "corpus", "zz" };

	/*if (argc != 3) {
		cerr << "uso: " << argv[0] << " #tarchivos #tmaximum" << endl;
		return 1;
	}*/
	int tiempo = 0;
	for (int i = 1 ; i < 6 ; i++){ // i varia el parametro p archivos
		for (int j = 1 ; j < 6 ; j++){  // j varia el parametro p maximum
			for (int k = 0 ; k < 30 ; k++){ // k varia las instancias
				auto start = std::chrono::high_resolution_clock::now();
				p = ConcurrentHashMap::maximum(j,k,l);
				auto finish = std::chrono::high_resolution_clock::now();
				tiempo = (std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() / 1000000 ) + tiempo;
			}
			cout << "Tiempo promedio con parametros: " << i << " " <<  j << " " <<  tiempo/30 <<  " ms" << endl;
			tiempo = 0;
		}		
	}

	//cout << "Tiempo promedio: " << tiempo/30 <<  " ms" << endl;
	//cout << "Tiempo tomado : "<< (std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() / 1000000 ) << " ms " << endl;
	//cout << p.first << " " << p.second << endl;

	return 0;
}

