#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctype.h>
#include "HashMap.hpp"

using namespace std;

typedef pair<string, unsigned int> kv_pair;
typedef list<kv_pair> kv_list;
typedef kv_list::iterator kv_list_it;
typedef kv_list::const_iterator kv_list_const_it;

HashMap::HashMap() {
    _size = 0;
    for (int i = 0; i < 26; ++i) {
        table[i] = kv_list();
    }
}

unsigned int HashMap::size() const {
    return _size;
}

void HashMap::addAndInc(const string &key) {
    // Busco el bucket correspondiente
    string valid_key = make_valid_key(key);
    int index = hash(valid_key);
    if (index < 0 || index > 25) {
        return;
    }
    kv_list &bucket = table[index];

    // Busco la palabra en su bucket
    kv_list_it it;
    for (it = bucket.begin(); it != bucket.end(); ++it) {
        if ((*it).first == valid_key) {
            break;
        }
    }

    if (it == bucket.end()) {
        // La palabra no estaba
        bucket.push_back(make_pair(valid_key, 1));
    }
    else {
        // La palabra ya estaba; incremento el valor en 1
        unsigned int value = (*it).second;
        (*it) = make_pair(valid_key, value + 1);
    }

    ++_size;
}

bool HashMap::member(const string &key) const {
    // Busco el bucket correspondiente
    string valid_key = make_valid_key(key);
    int index = hash(valid_key);
    if (index < 0 || index > 25) {
        return false;
    }
    kv_list const &bucket = table[index];

    // Me fijo si la palabra está
    bool res = false;
    for (kv_list_const_it it = bucket.begin();
        it != bucket.end();
        ++it)
    {
        if ((*it).first == valid_key) {
            res = true;
        }
    }

    return res;
}

kv_pair HashMap::maximum() const {
    kv_pair result = make_pair("", 0);

    for (int i = 0; i < 26; ++i) {
        kv_list const &bucket = table[i];

        for (kv_list_const_it it = bucket.begin();
            it != bucket.end();
            ++it)
        {
            if ((*it).second > result.second) {
                result = *it;
            }
        }
    }

    return result;
}


void HashMap::load(const string &filename) {
    fstream file(filename.c_str(), file.in);
    string read_word;

    if (!file.is_open()) {
        cerr << "Error al abrir el archivo '" << filename << "'" << endl;
        return;
    }

    while (file >> read_word) {
        addAndInc(read_word);
    }
    if (!file.eof()) {
        cerr << "Error al abrir el archivo '" << filename << "'" << endl;
    }
    file.close();
}

void HashMap::printAll() const {
    for (int i = 0; i < 26; ++i) {
        kv_list const &bucket = table[i];

        for (kv_list_const_it it = bucket.begin();
            it != bucket.end();
            ++it)
        {
            cout << "<" << (*it).first << "," << (*it).second << ">" << endl;
        }
    }
}

int HashMap::hash(const string &key) {
    // Devuelvo el índice correspondiente a la primera letra de la palabra

    if (key.size() == 0 || is_invalid_char(key[0])) {
        cerr << "Clave inválida" << endl;
        return -1;
    }

    return int(tolower(key[0])) - 97;
}

bool HashMap::is_invalid_char(char c) {
    return ! isalpha(c);
}

string HashMap::make_valid_key(const string &key) {
    string valid_key = key;

    valid_key.erase(
        remove_if(valid_key.begin(), valid_key.end(), is_invalid_char),
        valid_key.end()
    );

    transform(valid_key.begin(), valid_key.end(), valid_key.begin(), ::tolower);

    return valid_key;
}


// Iterador

HashMap::iterator HashMap::begin() {
    int i;
    for (i = 0; i < 26; ++i) {
        if (table[i].size() > 0) {
            break;
        }
    }
    if (i < 26) {
       return iterator(*this, i, table[i].begin(), 0);
    } else {
        return iterator(*this, 26, kv_list_const_it(), 0);
    }
}

HashMap::iterator HashMap::end() {
    return iterator(*this, 26, kv_list_const_it(), 0);
}

HashMap::iterator::iterator(
    const HashMap &hash_map,
    unsigned int bucket,
    kv_list_const_it it,
    unsigned int reps
) :
    hash_map(hash_map),
    bucket(bucket),
    it(it),
    reps(reps)
{}

string HashMap::iterator::operator*() {
    return (*it).first;
}

HashMap::iterator& HashMap::iterator::operator++() {
    if (bucket < 26) {
        // Siempre avanzo una repetición la palabra
        ++reps;
        if (reps == (*it).second) {
            // Si se acabaron las repeticiones, avanzo de palabra
            reps = 0;
            ++it;
            if (it == hash_map.table[bucket].end()) {
                // Si se acabaron las palabras del bucket, avanzo al siguiente
                // bucket que no esté vacío
                for (++bucket; bucket < 26; ++bucket) {
                    if (hash_map.table[bucket].size() > 0) {
                        it = hash_map.table[bucket].begin();
                        break;
                    }
                }
            }
        }
    }
    return *this;
}

HashMap::iterator HashMap::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool HashMap::iterator::operator==(const HashMap::iterator &other) const {
    return (&(hash_map) == &(other.hash_map)) && (
        (bucket > 25 && other.bucket > 25) || (
            (bucket == other.bucket) &&
            (it == other.it) &&
            (reps == other.reps)
        )
    );
}

bool HashMap::iterator::operator!=(const HashMap::iterator &other) const {
    return ! (*this == other);
}
