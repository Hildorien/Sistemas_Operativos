#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(int argc, char **argv) {
	pair<string, unsigned int> p;
	list<string> l = { "corpus", "corpus", "corpus", "corpus", "corpus" };

	if (argc != 3) {
		cerr << "uso: " << argv[0] << " #tarchivos #tmaximum" << endl;
		return 1;
	}
	auto start = std::chrono::high_resolution_clock::now();
	p = ConcurrentHashMap::maximum(atoi(argv[1]), atoi(argv[2]), l);
	auto finish = std::chrono::high_resolution_clock::now();
	cout << "Tiempo tomado : "<< (std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() / 1000000 ) << " ms " << endl;
	cout << p.first << " " << p.second << endl;

	return 0;
}

