#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(void) {
	pair<string, unsigned int> p;
	list<string> l = { "corpus", "corpus", "zz", "zz", "zz" };
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	int tiempo = 0;
	for (int i = 0 ; i < 30 ; i++){
	   	start = std::chrono::high_resolution_clock::now();
		p = ConcurrentHashMap::maximum(l);
		finish = std::chrono::high_resolution_clock::now();
		tiempo = (std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() / 1000000 ) + tiempo;
	}
	cout << "Tiempo promedio: " << tiempo/30 << " ms " <<  endl;
	cout << "Tiempo tomado : "<< (std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() / 1000000 ) << " ms " << endl;
	cout << p.first << " " << p.second << endl;

	return 0;
}
