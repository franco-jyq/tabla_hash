#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <stdbool.h>

/* *****************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* Se trata de una lista que contiene datos de tipo void*
 * (punteros genéricos).  La lista en sí está definida en el .c.  */

struct lista;  // Definición completa en lista.c.
typedef struct lista lista_t;

/* Se trata de una iterador externo que recorre los datos de una lista.
 * El itreador en sí está definido en el .c.  */

typedef struct lista_iter lista_iter_t; // Definicion completa en lista.c

/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una lista vacia.
lista_t* lista_crear(void);

// Devuelve verdadero si la lista tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Inserta un elemento al inicio de la lista.
// Pre: la lista fue creada.
// Post: el elemento fue insertado al inicio de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un elementosento al final de la lista.
// Pre: la lista fue creada.
// Post: el elemento fue insertado al final de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Borra el primer elemento de lista y lo devuelve, si la lista esta vacia devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvio y borro el primer elemento de la lista, la lista tiene un elemento menos, si
// la lista no estaba vacia.
void* lista_borrar_primero(lista_t *lista);

// Devuelve el primer elemento de la lista, si la lista esta vacia devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvio el primer elemento de la lista, si la lista no tenia elementos se devolvio NULL.
void* lista_ver_primero(const lista_t *lista);

// Devuelve el ultimo elemento de la lista, si la lista esta vacia devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvio el ultimo elemento de la lista, si la lista no tenia elementos se devolvio NULL.
void* lista_ver_ultimo(const lista_t* lista);

// Devuelve la cantidad de elementos que hay en la lista.
// Pre: la lista fue creada.
// Post: se devolvio el largo de la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

// Iterador interno de la lista, recibe una funcion visitar y un extra (opcional) y recorre todos
// los elementos de la lista.
// Pre: la lista fue creada.
// Post: se iteraron los datos de la lista, condicionados por la funcion visitar.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* *****************************************************************
 *                    PRIMITIVAS Del ITERADOR EXTERNO
 * *****************************************************************/

// Crea un iterador apuntando a la primera posicion del TDA.
// Post: el iterador fue creado.
lista_iter_t*	lista_iter_crear(lista_t *lista);
	
// Avanza una posicion sobre el tda si no hay una posicion siguiente devuelve false.
// Pre: el iterador fue creado.
// Post: (si la hay) se avanzo una posicion en el iterador.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el elemento el cual esta apuntando el iterador actualmente.
// Pre: el iterador fue creado.
// Post: se devolvio la posicion actual del iterador.
void* lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve true si la posicion actual esta afuera de la lista, false en caso contrario.
// Pre: el iterador fue creado.
// Post: se devolvio true si el iterador apunta afuera de la lista, false en caso contrario.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador.
// Pre: el iterador fue creado.
// Post: se destruyo el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un elemento detras de la posicion actual.
// Pre: el iterador fue creado.
// Post: el elemento fue insertado detras de la posicion actual.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra y devuelve el elemento al que esta apuntando el itrador actualmente, si no hay
// elementos devuelve NULL.
// Pre: el iterador fue creado.
// Post: el elemento ha sido borrado y devuelto al usuario.
void* lista_iter_borrar(lista_iter_t *iter);

#endif

	