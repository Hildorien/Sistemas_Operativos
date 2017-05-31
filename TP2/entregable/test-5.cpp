#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(int argc, char **argv) {
	pair<string, unsigned int> p;
	list<string> l = { "corpus", "corpus", "corpus", "corpus", "corpus" };

	if (argc != 3) {
		cerr << "uso: " << argv[0] << " #tarchivos #tmaximum" << endl;
		return 1;
	}
	p = ConcurrentHashMap::maximum(atoi(argv[1]), atoi(argv[2]), l);
	cout << p.first << " " << p.second << endl;

	return 0;
}

