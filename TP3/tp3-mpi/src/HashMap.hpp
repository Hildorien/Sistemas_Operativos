#ifndef _HASH_MAP_H
#define _HASH_MAP_H

#include <string>
#include <list>

class HashMap {
private:
    std::list< std::pair<std::string, unsigned int> > table[26];
    unsigned int _size;
    static int hash(const std::string &);
    static bool is_invalid_char(char);
    static std::string make_valid_key(const std::string &);

public:
    // Iterador de HashMap
    //   Va recorriendo una a una las palabras almacenadas en la primera
    //   componente de cada par; cada palabra es devuelta tantas veces
    //   como lo indica la segunda componente del par
    class iterator {
    private:
        friend HashMap;
        iterator(
            const HashMap &,
            unsigned int,
            std::list< std::pair<std::string, unsigned int> >::const_iterator,
            unsigned int
        );
        const HashMap &hash_map;
        unsigned int bucket;
        std::list< std::pair<std::string, unsigned int> >::const_iterator it;
        unsigned int reps;
    public:
        // Desreferencia el iterador (devuelve la palabra a la que apunta)
        std::string operator*();

        // Avanza una posición el iterador y lo devuelve incrementado
        iterator & operator++();

        // Avanza una posición el iterador y lo devuelve sin incrementar
        iterator operator++(int);

        // Comparación de iteradores
        bool operator==(const iterator &) const;
        bool operator!=(const iterator &) const;
    };

    // Crea un HashMap vacío
    HashMap();

    // Devuelve la cantidad de palabras almacenadas (cada repetición cuenta)
    unsigned int size() const;

    // Si key existe, incrementa su valor, si no existe, crea el par (key, 1)
    void addAndInc(const std::string &key);

    // Devuelve true ssi el par (key, x) pertenece al HashMap para algún x
    bool member(const std::string &key) const;

    // Devuelve un par (key, x) cuyo valor de x es máximo dentro del HashMap
    std::pair<std::string, unsigned int> maximum() const;

    // Toma un archivo de texto y carga en el HashMap las palabras del mismo
    void load(const std::string &filename);

    // Para debug - Imprime todos los pares almacenados en el HashMap
    void printAll() const;

    // Crea un iterador apuntando al comienzo del HashMap
    iterator begin();

    // Crea un iterador apuntando al final del HashMap
    //   (No desreferenciar - comportamiento indefinido)
    iterator end();
};

#endif /* _HASH_MAP_H */
