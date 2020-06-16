#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct elementos{
	void* dato;
	char* clave;
};


struct hash{
    lista_t** lista;
    size_t cantidad;
    size_t tam;
    void* puntero_a_funcion;
    hash_destruir_dato_t destruir_dato;
}


struct hash_iter;

