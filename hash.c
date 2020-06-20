#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "hash.h"
#define CONSTANTE_DE_REDIMENSION 2
#define TAMANIO_INICIAL 37
// Fuente funcion de hash: http://www.azillionmonkeys.com/qed/hash.html
// Definiciones para la funcion de hash 
#include "pstdint.h"       /* Replace with <stdint.h> if appropriate */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__TURBOC__)
#define get16bits(d) (*((const uint16_t *)(d)))
#endif

#if !defined(get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) + (uint32_t)(((const uint8_t *)(d))[0]))
#endif

/* *****************************************************************
 *                    FUNCION DE HASHING
 * ******************************************************************/

uint32_t SuperFastHash(const char *data, int len){
    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL)
        return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (; len > 0; len--)
    {
        hash += get16bits(data);
        tmp = (get16bits(data + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += 2 * sizeof(uint16_t);
        hash += hash >> 11;
    }

    /* Handle end cases */
    switch (rem)
    {
    case 3:
        hash += get16bits(data);
        hash ^= hash << 16;
        hash ^= ((signed char)data[sizeof(uint16_t)]) << 18;
        hash += hash >> 11;
        break;
    case 2:
        hash += get16bits(data);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1:
        hash += (signed char)*data;
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
    void *dato;
    char *clave;
} campo_hash_t;

campo_hash_t *crear_campo(char *clave, void *dato){
    campo_hash_t *campo = malloc(sizeof(campo_hash_t));
    if (!campo) return NULL;
    campo->clave = clave;
    campo->dato = dato;
    return campo;
}

void destruir_campo(void *campo){
    free(((campo_hash_t*)campo)->clave);
    free(campo);
}

struct hash{
    lista_t **lista;
    size_t cantidad;
    size_t tam;
    hash_destruir_dato_t destruir_dato;
};

/* *****************************************************************
 *                    FUNCIONES Del TDA HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t *hash = malloc(sizeof(hash_t));
    if (!hash)return NULL;
    hash->tam = TAMANIO_INICIAL;
    lista_t **arreglo_listas = malloc(sizeof(lista_t *) * (hash->tam));
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

void destruir_tabla(lista_t** tabla,size_t k){
    for (size_t i = 0;i < k;i++){
        lista_destruir(tabla[i],destruir_campo);
    }
    free(tabla);
}

bool hash_redimensionar(hash_t *hash){
    size_t factor_carga = hash->cantidad / hash->tam;
    if (factor_carga == 2 || factor_carga == 3 || (factor_carga < 2 && hash->tam == TAMANIO_INICIAL)) return true; 
    size_t nuevo_tam = 0;
    if (factor_carga >= 3) nuevo_tam = hash->tam * CONSTANTE_DE_REDIMENSION;
    if (factor_carga < 2) nuevo_tam = hash->tam / CONSTANTE_DE_REDIMENSION;
    lista_t **nueva_tabla = malloc(sizeof(lista_t *) * nuevo_tam);
    if (!nueva_tabla) return NULL;
    for (size_t i = 0; i < nuevo_tam; i++){
        nueva_tabla[i] = lista_crear();
        if (!nueva_tabla[i]){
            destruir_tabla(nueva_tabla, i);
            return false;
        }
    }
    hash_iter_t *iter = hash_iter_crear(hash);
    if (!iter){
        destruir_tabla(nueva_tabla, nuevo_tam);
        return false;
    }
    while (!hash_iter_al_final(iter)){
        const char *clave = (hash_iter_ver_actual(iter));
        char *copia_clave = malloc(sizeof(char) * (strlen(clave) + 1));
        if (!copia_clave){
            destruir_tabla(nueva_tabla, nuevo_tam);
            return false;
        }
        strcpy(copia_clave, clave);
        void *dato = hash_obtener(hash, clave);
        campo_hash_t *nuevo_campo = crear_campo(copia_clave, dato);
        if (!nuevo_campo){
            destruir_tabla(nueva_tabla, nuevo_tam);
            return false;
        }
        size_t i = SuperFastHash(clave, (int)strlen(clave)) % nuevo_tam;
        if (!lista_insertar_primero(nueva_tabla[i], nuevo_campo)){
            destruir_tabla(nueva_tabla, nuevo_tam);
            return false;
        }
        hash_iter_avanzar(iter);
    }
    hash_iter_destruir(iter);
    destruir_tabla(hash->lista, hash->tam);
    hash->lista = nueva_tabla;
    hash->tam = nuevo_tam;
    return true;
}

bool comparar_claves(const char *clave1, const char *clave2){
    if (strcmp(clave1, clave2) == 0) return true;
    return false;
}

bool esta_el_elemento(const char *clave, void **dato, lista_iter_t *iter){
    if (!iter)return false;
    while (!lista_iter_al_final(iter)) {
        if (comparar_claves(((campo_hash_t *)(lista_iter_ver_actual(iter)))->clave, clave)) {
            *dato = ((campo_hash_t *)(lista_iter_ver_actual(iter)))->dato;
            return true;
        }
        lista_iter_avanzar(iter);
    }
    return false;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    size_t i = SuperFastHash(clave, (int)strlen(clave)) % hash->tam;
    lista_iter_t *iter = lista_iter_crear(hash->lista[i]);
    void *nada = NULL;
    if (esta_el_elemento(clave, &nada, iter)){
        if (hash->destruir_dato){
            hash->destruir_dato(((campo_hash_t *)(lista_iter_ver_actual(iter)))->dato);
        }
        ((campo_hash_t *)(lista_iter_ver_actual(iter)))->dato = dato;
        lista_iter_destruir(iter);
        return true;
    }
    lista_iter_destruir(iter);
    char *copia_clave = malloc(sizeof(char) * (strlen(clave) + 1));
    if (!copia_clave) return false;
    strcpy(copia_clave, clave);
    campo_hash_t *nuevo_campo = crear_campo(copia_clave, dato);
    if (!nuevo_campo)return false;
    if (!lista_insertar_primero(hash->lista[i], nuevo_campo)){
        destruir_campo(nuevo_campo);
        return false;
    }
    hash->cantidad++;
    hash_redimensionar(hash);
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    size_t i = SuperFastHash(clave, (int)strlen(clave)) % hash->tam;
    void *dato = NULL;
    lista_iter_t *iter = lista_iter_crear(hash->lista[i]);
    if (!iter)return NULL;
    if (esta_el_elemento(clave, &dato, iter)){
        destruir_campo((campo_hash_t *)(lista_iter_ver_actual(iter)));
        lista_iter_borrar(iter);
        hash->cantidad--;
    }
    lista_iter_destruir(iter);
    hash_redimensionar(hash);
    return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    size_t i = SuperFastHash(clave, (int)strlen(clave)) % hash->tam;
    void *dato = NULL;
    lista_iter_t *iter = lista_iter_crear(hash->lista[i]);
    if (!iter) return NULL;
    esta_el_elemento(clave, &dato, iter);
    lista_iter_destruir(iter);
    return dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t i = SuperFastHash(clave, (int)strlen(clave)) % hash->tam;
    bool pertenece = false;
    lista_iter_t *iter = lista_iter_crear(hash->lista[i]);
    if (!iter)return false;
    void *nada = NULL;
    pertenece = esta_el_elemento(clave, &nada, iter);
    lista_iter_destruir(iter);
    return pertenece;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    for (size_t i = 0; i < hash->tam; i++){
        while (!lista_esta_vacia(hash->lista[i])) {
            if (hash->destruir_dato){
                hash->destruir_dato(((campo_hash_t *)lista_ver_primero(hash->lista[i]))->dato);
            }
            destruir_campo((campo_hash_t *)(lista_borrar_primero(hash->lista[i])));
        }
        lista_destruir(hash->lista[i], NULL);
    }
    free(hash->lista);
    free(hash);
}

/* *****************************************************************
 *                    FUNCIONES Del Iter Hash
 * *****************************************************************/

struct hash_iter{
    lista_iter_t *lista_iter;
    const hash_t *hash;
    size_t iterados;
    size_t cont;
};

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t *hash_iter = malloc(sizeof(hash_iter_t));
    if (!hash_iter)return NULL;
    hash_iter->cont = 0;
    while (lista_esta_vacia(hash->lista[hash_iter->cont]) && hash_iter->cont < hash->tam - 1){
        hash_iter->cont++;
    }
    lista_iter_t *lista_iter = lista_iter_crear(hash->lista[hash_iter->cont]);
    if (!lista_iter)return NULL;
    hash_iter->lista_iter = lista_iter;
    hash_iter->hash = hash;
    hash_iter->iterados = 0;
    return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return false;
    lista_iter_avanzar(iter->lista_iter);
    iter->iterados++;
    if (lista_iter_al_final(iter->lista_iter)){
        iter->cont++;
        while (iter->cont < iter->hash->tam - 1 && lista_esta_vacia(iter->hash->lista[iter->cont])){
            iter->cont++;
        }
        lista_iter_destruir(iter->lista_iter);
        iter->lista_iter = NULL;
        if (!hash_iter_al_final(iter)) {
            lista_iter_t *nuevo_lista_iter = lista_iter_crear(iter->hash->lista[iter->cont]);
            if (!nuevo_lista_iter) return false;
            iter->lista_iter = nuevo_lista_iter;
        }
    }
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return NULL;
    return ((campo_hash_t *)(lista_iter_ver_actual(iter->lista_iter)))->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
    return (iter->iterados == iter->hash->cantidad);
}

void hash_iter_destruir(hash_iter_t *iter){
    if (iter->lista_iter)lista_iter_destruir(iter->lista_iter);
    free(iter);
}
