#include "lista.h"
#include <stdlib.h>

/* Definición del struct nodo.
 */
typedef struct nodo{
	void* dato;			// Dato del nodo.
	struct nodo* prox;	// Referencia al siguiente nodo.
}nodo_t;


nodo_t* crear_nodo(void* valor){ 
	nodo_t* nodo = malloc(sizeof(nodo_t));		// Pido memoria para un nodo.
	if(!nodo){
		return false;							// Si el nodo no se pudo crear devuelvo false.
	}
	nodo->dato = valor;							// Hago que el dato del nodo sea igual a valor.
	nodo->prox = NULL;							// Hago que el proximo del nodo sea NULL.
	return nodo;								// Devuelvo el nodo.
}
/* Definicion del struct lista.
*/
struct lista{
	nodo_t* prim;	// Primer nodo de la lista.
	nodo_t* ult;	// Ultimo nodo de la lista.
	size_t largo;	// Largo de la lista.
};

/* *****************************************************************
 *                    FUNCIONES Del TDA LISTA
 * *****************************************************************/

lista_t* lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));	// Pido memoria para una lista .
	if(!lista){
		return false;							// Si el la lista no se pudo crear devuelvo false.
	}
	lista->prim = NULL;							// Hago que el primero de la lista sea NULL.
	lista->ult = NULL;							// Hago que el ultimo de la lista sea NULL.
	lista->largo = 0;							// Hago que el largo de la lista sea 0.
	return lista;								// Devuelvo la lista.
}

bool lista_esta_vacia(const lista_t *lista){
	return lista->largo == 0;					// Devuelvo el booleano de si el largo de la lista es 0.
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = crear_nodo(dato);			// Creo un nuevo nodo con el dato.
	if(!nodo){									// Si el nodo no se pudo crear devuelvo false.
		return false;
	}
	if (lista_esta_vacia(lista)){
		lista->prim = nodo;						// Hago que el nodo sea el primero de la lista.
		lista->ult = nodo;						// Hago que el nodo sea el ultimo de la lista.
	}
	else{
		nodo->prox = lista->prim;				// Hago que el proximo del nodo apunte al primero de la lista.
		lista->prim = nodo;						// Hago que el nodo sea el primero de la lista.
	}
	lista->largo ++;							// Incremento el largo de la lista.
	return true;								// Devuelvo true.
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo = crear_nodo(dato);			// Creo un nuevo nodo con el dato.
	if(!nodo){									// Devuelvo false si le nodo no se pudo crear.
		return false;
	}
	if (lista_esta_vacia(lista)){				// Si la lista esta vacia hago que el
		lista->prim = nodo;						// 	nodo sea el primero.
	}
	else{
		lista->ult->prox = nodo;				// Hago que nodo sea el proximo del ultimo actual.
	}
	lista->ult = nodo;							// Hago que el ultimo sea el nuevo nodo.
	lista->largo ++;							// Incremento el largo de la lista.
	return true;								// Devuelvo true.
}

void* lista_borrar_primero(lista_t *lista){
	if(lista_esta_vacia(lista)){				// Si la lista esta vacia devuelvo NULL.
		return NULL;
	}
	void* dato = lista->prim->dato;				// Guardo el dato del primero.
	nodo_t* nuevo_prim = lista->prim->prox;		// Guardo el nodo proximo al primero.
	free(lista->prim);							// Libero la memoria del primero nodo de la lista.
	lista->prim = nuevo_prim;					// Declaro a nuevo_prim como primero de la lista.
	lista->largo --;							// Decremento el largo de la lista.
	return dato;								// Devuelvo le dato.
}

void* lista_ver_primero(const lista_t *lista){
	if(lista_esta_vacia(lista)){				// Si la lista esta vacia devuelvo NULL.
		return NULL;
	}
	return lista->prim->dato;					// Devuelvo el dato del primero de la lista. 
}	

void* lista_ver_ultimo(const lista_t* lista){
	if(lista_esta_vacia(lista)){				// Si la lista esta vacia devuelvo NULL.
		return NULL;
	}
	return lista->ult->dato;					// Devuelvo el dato del ultimo de la lista.
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;						// Devuelvo el largo de la lista.
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while(!lista_esta_vacia(lista)){				// Itero hasta que la lista este vacia.
		void* dato = lista_borrar_primero(lista);	// Me guardo el dato del nodo que quiero borrar.
		if(destruir_dato != NULL){
			destruir_dato(dato);					// Le aplico la funcion destruir al dato del nodo actual.
		}
	}
	free(lista);									// Libero la memoria que pedi para guardar la lista	.
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	nodo_t* act = lista->prim;						// Guardo el primero de la lista en act.
	while(act != NULL){								// Itero hasta que el actual valga NULL.
		if(visitar(act->dato, extra) == false){		// Le aplico la funcion visitar al dato del nodo actual.
			break;									// Termino la iteracion si visitar es false.
		}
		act = act->prox;							// Actualizo la posicion actual.
	}													
}

/* Definicion del struct lista_iter.
*/
struct lista_iter{
	nodo_t* act;	// Nodo de referencia a la posicion actual.
	nodo_t* ant;	// Nodo de referencia  al a posicion anterior a la actual.
	lista_t* lista; // Puntero a la lista original.
};

/* *****************************************************************
 *                    FUNCIONES Del ITERADOR EXTERNO
 * *****************************************************************/

lista_iter_t* lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));	// Pido memoria para guardar un iter
	if(!iter){
		return NULL;
	}
	iter->lista = lista;						// Guardo la lista en el iterador.
	iter->act = lista->prim;					// Asigno el actual al primero de la lista.
	iter->ant = NULL;							// Hago que el anterior apunte a NULL.
	return iter;								// Devuelvo el iterador.
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return iter->act == NULL;					// Devuelvo true si el actual apunta a NULL.
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if(lista_iter_al_final(iter)){				// Devuelvo false si el iterador esta al final.
		return false;	
	}
	iter->ant = iter->act;						// Actualizo anterior y actual del iterador.
	iter->act = iter->act->prox;
	return true;							
}

void* lista_iter_ver_actual(const lista_iter_t *iter){
	if(lista_iter_al_final(iter)){ 				// Devuelvo NULL si el iterador esta al final.
		return NULL;							
	}
	return iter->act->dato;						// Devuelvo el dato del nodo actual.
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);									//Libero la memoria que pedi para el iterador.
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* nuevo = crear_nodo(dato);			// Creo un nuevo nodo.	
	if(!nuevo){									// Si no se pudo crear devuelvo false.
		return false;
	}
	if(lista_esta_vacia(iter->lista)){			// Entro si la lista esta vacia.
		iter->lista->prim = nuevo;				// Hago que el nuevo sea el primero de la lista.
		iter->lista->ult = nuevo;				// Hago que el nuevo sea el ultimo de la lista.
	}
	else{
		nuevo->prox = iter->act;				// Hago que el proximo de nuevo sea el actual.
		if(iter->act == iter->lista->prim){		// Entro si el actual coincide con el primer elemento.
			iter->lista->prim = nuevo;			// Hago que nuevo sea el primero de la lista.
		}
		else if(lista_iter_al_final(iter)){		// Entro si el actual esta al final.
			iter->lista->ult = nuevo;			// Hago que nuevo se el ultimo de la lista.
			iter->ant->prox = nuevo;			// Hago que el proximo del anterior sea el nuevo.
		}
		else{
			iter->ant->prox = nuevo;			// Hago que el proximo del anterior sea el nuevo.
		}
	}
	iter->act = nuevo;							// Hago que nuevo sea el actual.
	iter->lista->largo ++;						// Incremento el largo de la lista.
	return true;								// Devuelvo true.
}


void* lista_iter_borrar(lista_iter_t *iter){
	if(lista_esta_vacia(iter->lista) || lista_iter_al_final(iter)){		// Si la lista esta vacia o el actual 
		return NULL;													//  esta fuera de la misma devuelvo NULL.
	}
	if(iter->act == iter->lista->prim){				//Si el actual era el primero de la lista actualizo
		iter->act = iter->act->prox;				// el actual y devuelvo lista_borrar_primero.
		return lista_borrar_primero(iter->lista);	
	}
	void* dato = iter->act->dato;					// Guardo el dato antes de borrar el nodo.
	nodo_t* nodo_a_borrar = iter->act;				// Guardo el nodo que voy a borrar para no perder la referencia.
	iter->ant->prox = iter->act->prox;				// Hago que el anterior apunte al siguiente del actual.
	if(iter->act == iter->lista->ult){				// Si el elemento era el ultimo declaro el que quedo como nuevo ultimo.
		iter->lista->ult = iter->ant;
	}
	free(nodo_a_borrar);							// Libero la memoria del nodo.
	iter->act = iter->ant->prox;					// Actualizo el actual del iterador.
	iter->lista->largo --;							// Actualizo el largo de la lista.
	return dato;									// Devuelvo el dato.

}