#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "hash.h"
#define TAMANIO_INICIAL 17 // tiene que ser un numero primo
#include "pstdint.h" /* Replace with <stdint.h> if appropriate */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif



/* *****************************************************************
 *                    FUNCION DE HASHING
 * ******************************************************************/

uint32_t SuperFastHash (const char * data, int len) {
    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

/* *****************************************************************
 *                   ESTRUCTURAS
 * *****************************************************************/


typedef struct campo_hash{
	void* dato;
	const char* clave; //  --  tuve q agregar const por la misma razon del warning de abajo
} campo_hash_t;

campo_hash_t* crear_campo (const char* clave, void* dato){ // -- tuve q agregar const por un warning 
    campo_hash_t* elemento = malloc(sizeof(campo_hash_t));
    if (!elemento) return NULL;
    elemento->clave = clave;  
    elemento->dato = dato;  
    return elemento;
}

void destruir_campo (campo_hash_t* campo){ 
    free(campo);                            
}


struct hash{
    lista_t** lista;
    size_t cantidad;
    size_t tam;
    hash_destruir_dato_t destruir_dato;
};



/* *****************************************************************
 *                    FUNCIONES Del TDA HASH
 * *****************************************************************/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;
	hash->tam = TAMANIO_INICIAL;				
	lista_t** arreglo_listas = malloc(sizeof(lista_t*) * (hash->tam));
	if (!arreglo_listas){
        free(hash);
        return NULL;
    } 
	for (size_t i = 0; i < hash->tam; i++){
		arreglo_listas[i] = lista_crear();    
		if (!arreglo_listas[i]){
            hash_destruir(hash);
            return NULL;
        }    
	}

	hash->lista = arreglo_listas;		
    hash->cantidad = 0;
	hash->destruir_dato = destruir_dato;
	return hash;
}
// Compara dos claves, creo que en string.h no hay una funcion equivalente -- si solo qeremos saber si son iguales creo q nos sirve strcmp()
bool comparar_claves (const char* clave1, const char* clave2){
    /* qedaria algo asi:
    if(strcmp(clave1, clave2) == 0) return true;
    return false;
    */
    for (size_t i = 0;i < strlen(clave1);i++){
        if (clave1[i] != clave2[i]){
            return false;
        }        
    }
    if (strlen(clave1) == strlen(clave2)){
        return true;
    }
    return false;
}    



bool hash_guardar(hash_t* hash, const char* clave, void* dato){
    size_t i = SuperFastHash(clave, strlen(clave)) % hash->tam;
    bool repetida = false;
    lista_iter_t* iter = lista_iter_crear(hash->lista[i]);
    if (!iter) return false;
    while(!lista_iter_al_final(iter)){
        if (comparar_claves(((campo_hash_t*)(lista_iter_ver_actual(iter)))->clave,clave)){
            ((campo_hash_t*)(lista_iter_ver_actual(iter)))->dato = dato;
            repetida = true;    
        }
    }
    lista_iter_destruir(iter);
    if (repetida) return true;
    campo_hash_t* nuevo_campo = crear_campo(clave, dato);
    if (!nuevo_campo) return false;
    if (!lista_insertar_primero(hash->lista[i],nuevo_campo)){
        destruir_campo(nuevo_campo);
        return false;
    } 
    return true;
}
// borrar,obtener y pertenece son lo mismo salvo 3 lineas de codigo
void* hash_borrar(hash_t *hash, const char *clave){
    size_t i = SuperFastHash(clave, strlen(clave)) % hash->tam;
    void* dato = NULL;
    lista_iter_t* iter = lista_iter_crear(hash->lista[i]);
    if (!iter) return NULL;
    while(!lista_iter_al_final(iter)){
        if (comparar_claves(((campo_hash_t*)(lista_iter_ver_actual(iter)))->clave,clave)){
            dato = ((campo_hash_t*)(lista_iter_ver_actual(iter)))->dato;
            hash->destruir_dato(((campo_hash_t*)(lista_iter_ver_actual(iter)))->dato);
            destruir_campo((campo_hash_t*)(lista_iter_ver_actual(iter)));        
        }
    }
    lista_iter_destruir(iter);
    return dato;
}


void* hash_obtener(const hash_t *hash, const char *clave){
    size_t i = SuperFastHash(clave, strlen(clave)) % hash->tam;
    void* dato = NULL;
    lista_iter_t* iter = lista_iter_crear(hash->lista[i]);
    if (!iter) return NULL;
    while(!lista_iter_al_final(iter)){
        if (comparar_claves(((campo_hash_t*)(lista_iter_ver_actual(iter)))->clave,clave)){
            dato = ((campo_hash_t*)(lista_iter_ver_actual(iter)))->dato;        
        }
    }
    lista_iter_destruir(iter);
    return dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t i = SuperFastHash(clave, strlen(clave)) % hash->tam;
    bool pertenece = false;
    lista_iter_t* iter = lista_iter_crear(hash->lista[i]);
    if (!iter) return false;
    while(!lista_iter_al_final(iter)){
        if (comparar_claves(((campo_hash_t*)(lista_iter_ver_actual(iter)))->clave,clave)){
            pertenece = true;        
        }
    }
    lista_iter_destruir(iter);
    return pertenece;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    for (size_t i = 0;i < hash->tam;i++){
        while (!lista_esta_vacia(hash->lista[i])){ 
            hash->destruir_dato(((campo_hash_t*)lista_ver_primero(hash->lista[i]))->dato);
            destruir_campo((campo_hash_t*)(lista_borrar_primero(hash->lista[i])));
        }
        lista_destruir(hash->lista[i],NULL);
    }
    free(hash->lista);
    free(hash);
}

/* *****************************************************************
 *                    FUNCIONES Del Iter Hash
 * *****************************************************************/


struct hash_iter{
    lista_iter_t* lista_iter;    
    hash_t* hash;
    size_t cont;
};


hash_iter_t* hash_iter_crear(const hash_t *hash){
    hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
    if(!hash_iter) return NULL;
    lista_iter_t* lista_iter =  lista_iter_crear(hash->lista[0]);
    if(!lista_iter) return NULL;
    hash_iter->lista_iter = lista_iter;
    hash_iter->hash = hash;
    hash_iter->cont = 0;
    return hash_iter;
}


bool hash_iter_avanzar(hash_iter_t *iter){    
    if(hash_iter_al_final(iter))return false;
    
    if(lista_iter_al_final(iter->lista_iter)){
        lista_iter_destruir(iter->lista_iter);
        iter->cont ++;  
        lista_iter_t* nuevo_lista_iter = lista_iter_crear(iter->hash->lista[iter->cont]);
        if (!nuevo_lista_iter) return false; // faltaba esto creo
        iter->lista_iter = nuevo_lista_iter;     
    }
    
    lista_iter_avanzar(iter->lista_iter);
    return true;
}

const char* hash_iter_ver_actual(const hash_iter_t *iter){
    return lista_iter_ver_actual(iter->lista_iter);        // no estaria devolviendo todo el campo aca?
}


bool hash_iter_al_final(const hash_iter_t *iter){
    return (iter->cont == iter->hash->tam);
}


void hash_iter_destruir(hash_iter_t* iter){
    lista_iter_destruir(iter->lista_iter);
    free(iter);
}




