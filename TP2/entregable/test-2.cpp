#include <iostream>
#include "ConcurrentHashMap.hpp"

using namespace std;

int main(void) {
	ConcurrentHashMap h;
	int i;
	
	h.count_words("corpus"); // antes estaba h = count_word("corpus") y me tiraba error porque no reconocia la funcion, ahora con este cambio anda
	for (i = 0; i < 26; i++) {
		for (auto it = h.tabla[i]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
			auto t = it.Siguiente();
			cout << t.first << " " << t.second << endl;
		}
	}

	return 0;
}

