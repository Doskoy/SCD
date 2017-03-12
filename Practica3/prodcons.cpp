#include "mpi.h"
#include <iostream>
#include <math.h>

#define Tag_Prod 	0					// Identificador de los productores
#define Tag_Buff 	1 					// Identificador del buffer
#define Tag_Cons 	2 					// Identificador de los consumidores
#define N_Prods		5					// Total de productores
#define N_Cons		4					// Numero de consumidores
#define Buffer 		N_Prods				
#define Iters 		20					// Total de iteraciones
#define Tam 		5 					// Tamaño del buffer de intercambio de datos

using namespace std;

/*
** Para asegurarme de que todas las hebras hagan alguna iteracion hago que los
** productores se repartan las iteracciones de la forma mas equitativa posible
*/
void productor(){
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (unsigned int i=rank; i<Iters; i += N_Prods){			
		cout << " El Productor "<< rank << " produce valor "<< i << endl << flush;
		MPI_Ssend( &i, 1, MPI_INT, Buffer, Tag_Prod, MPI_COMM_WORLD );
	}

	cout << " FIN PRODUCTOR " << rank << endl << flush;
}

/* En los consumidores hago igual que con los productores, distribuyo el numero 
** de datos que pueden consumir cada uno */
void consumidor(){
	int value;								
	int peticion=1; 						// No importa el valor que pongamos ya que no trabajo con el valor sino con los tag						
	int rank;
	int tot = Iters / N_Cons;				// Los consumidores se repartiran que tienen que consumir cada uno

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	float raiz; 

	MPI_Status status;

	for (unsigned int i=0; i<tot; i++){
		MPI_Ssend(&peticion,1, MPI_INT, Buffer, Tag_Cons, MPI_COMM_WORLD);
		MPI_Recv(&value, 1, MPI_INT, Buffer, Tag_Buff, MPI_COMM_WORLD,&status );
		cout << " El Consumidor " << rank <<" recibe valor "<< value <<" de Buffer "<< endl << flush;
		raiz=sqrt(value);
	}

	cout << " FIN CONSUMIDOR " << rank << endl << flush;
}

/* En el buffer lo unico que hago es que mientras que el buffer no este lleno o vacio, miro
** de quien proviene el mensaje, distinguiendo 2 casos:
** 		- Mensaje del productor ==> me voy por la rama 0, y en ella añade ese dato al buffer
** 		- Mensaje del consumidor ==> me voy por la rama 1, y en ella se le proporciona
**		  al consumidor ese dato y se saca del buffer
** Ahora bien, si el buffer esta vacio se obliga a ir a la rama 0 en la que espera mensajes de productores
** y si el buffer esta lleno se obliga ir a la rama 1 en la que espera mensajes de consumidores */
void buffer(){
	int value[Tam];
	int peticion;
	int rama;
	int pos = 0;

	MPI_Status status;

	int tot = 2*Iters;
	for (unsigned int i=0; i<tot; i++){
		if (pos == 0)
			rama = 0;					//No hay datos, el consumidor no puede consumir
		else if ( pos == Tam)
			rama = 1;					//No hay huecos, el productor no puede producir
		else{
			MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD,&status);

			if(status.MPI_TAG == Tag_Prod) //Miro la etiqueta del emisor y en caso de que sea de 
				rama = 0;				   //productor lo mando a la rama 0 o a la 1 si no lo es	
			else
				rama = 1;
		}
		switch(rama){
			case 0:
				MPI_Recv(&value[pos], 1, MPI_INT, MPI_ANY_SOURCE, Tag_Prod, MPI_COMM_WORLD, &status); // Se reciben los mensajes que vengan
				cout << " Buffer recibe " << value[pos] << " de Prod. " << endl << flush;		  // de una fuente con la etiqueta especificada
				pos++;
				break;
			case 1:
				MPI_Recv(&peticion, 1, MPI_INT, MPI_ANY_SOURCE, Tag_Cons, MPI_COMM_WORLD, &status);
				MPI_Ssend(&value[pos-1],1,MPI_INT,status.MPI_SOURCE, Tag_Buff, MPI_COMM_WORLD);
				cout << " Buffer envia " << value[pos-1] << " a Cons." << endl << flush;
				pos--;
				break;
		}
		 
	}
}

int main(int argc, char *argv[]) {
	int rank,size;

	MPI_Init( &argc, &argv );

	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	if (rank < N_Prods) 		// Dado que los procesos 0...4 son productores si rank es menor que 5 llama a productor
		productor();
	else if (rank > N_Prods) 	// Dado que los procesos 6...9 son consumidores si rank es mayor que 5 llama a consumidor
		consumidor();
	else
		buffer();				// Si no es ninguno de los casos anteriores rank == 5 y llama a buffer

	MPI_Finalize( );
	return 0;
} 


/*
--------------------------Listado de salida----------------------------

 El Productor 1 produce valor 1
 El Productor 2 produce valor 2
 Buffer recibe 1 de Prod. 
 El Consumidor 7 recibe valor 1 de Buffer 
 Buffer envia 1 a Cons.
 El Productor 1 produce valor 6
 Buffer recibe 2 de Prod. 
 Buffer envia 2 a Cons.
 Buffer recibe 6 de Prod. 
 El Consumidor 6 recibe valor 2 de Buffer 
 El Productor 2 produce valor 7
 El Consumidor 7 recibe valor 6 de Buffer 
 El Productor 1 produce valor 11
 Buffer envia 6 a Cons.
 Buffer recibe 11 de Prod. 
 El Productor 1 produce valor 16
 El Consumidor 6 recibe valor 11 de Buffer 
 Buffer envia 11 a Cons.
 Buffer recibe 7 de Prod. 
 El Productor 2 produce valor 12
 El Consumidor 7 recibe valor 7 de Buffer 
 Buffer envia 7 a Cons.
 Buffer recibe 16 de Prod. 
 FIN PRODUCTOR 1
 Buffer recibe 12 de Prod. 
 El Productor 2 produce valor 17
 El Consumidor 6 recibe valor 12 de Buffer 
 Buffer envia 12 a Cons.
 Buffer recibe 17 de Prod. 
 Buffer envia 17 a Cons.
 FIN PRODUCTOR 2
 El Consumidor 7 recibe valor 17 de Buffer 
 Buffer envia 16 a Cons.
 El Consumidor 6 recibe valor 16 de Buffer 
 El Productor 0 produce valor 0
 Buffer recibe 0 de Prod. 
 Buffer envia 0 a Cons.
 El Consumidor 6 recibe valor 0 de Buffer 
 FIN CONSUMIDOR 6
 El Productor 0 produce valor 5
 Buffer recibe 5 de Prod. 
 El Consumidor 7 recibe valor 5 de Buffer 
 FIN CONSUMIDOR 7
 Buffer envia 5 a Cons.
 El Productor 0 produce valor 10
 El Productor 4 produce valor 4
 Buffer recibe 10 de Prod. 
 Buffer recibe 4 de Prod. 
 Buffer recibe 9 de Prod. 
 Buffer recibe 14 de Prod. 
 Buffer recibe 19 de Prod. 
 El Productor 4 produce valor 9
 El Productor 4 produce valor 14
 El Productor 4 produce valor 19
 FIN PRODUCTOR 4
 El Productor 0 produce valor 15
 Buffer envia 19 a Cons.
 Buffer recibe 15 de Prod. 
 Buffer envia 15 a Cons.
 El Consumidor 8 recibe valor 15 de Buffer 
 El Consumidor 8 recibe valor 14 de Buffer 
 El Consumidor 8 recibe valor 9 de Buffer 
 El Consumidor 8 recibe valor 4 de Buffer 
 FIN CONSUMIDOR 8
 Buffer envia 14 a Cons.
 Buffer envia 9 a Cons.
 Buffer envia 4 a Cons.
 FIN PRODUCTOR 0
 El Productor 3 produce valor 3
 Buffer envia 10 a Cons.
 El Consumidor 9 recibe valor 10 de Buffer 
 Buffer recibe 3 de Prod. 
 El Productor 3 produce valor 8
 Buffer envia 3 a Cons.
 Buffer recibe 8 de Prod. 
 El Consumidor 9 recibe valor 3 de Buffer 
 El Productor 3 produce valor 13
 El Productor 3 produce valor 18
 FIN PRODUCTOR 3
 Buffer recibe 13 de Prod. 
 Buffer recibe 18 de Prod. 
 Buffer envia 18 a Cons.
 El Consumidor 9 recibe valor 18 de Buffer 
 Buffer envia 13 a Cons.
 El Consumidor 9 recibe valor 13 de Buffer 
 Buffer envia 8 a Cons.
 El Consumidor 9 recibe valor 8 de Buffer 
 FIN CONSUMIDOR 9

*/