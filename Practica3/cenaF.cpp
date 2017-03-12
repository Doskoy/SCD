#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

/* Para solucionar la posibilidad del interbloqueo en los filosofos, basta que alguno de ellos
** en vez de coger primero su tenedor de la derecha coja el de la izquierda*/
void Filosofo(int id, int nprocesos){
	int izq;
	int der;

	int pet_tenedor = 1;
	int solicitud_tenedor = 0;

	/* Segun si son los filosofos pares o impares cogen primero tenedor derecha o izquierda */
	if(id % 4){
		izq=(id+1)%nprocesos;
		der=(id-1+nprocesos)%nprocesos;
	}
	else{
		izq=(id-1+nprocesos)%nprocesos;
		der=(id+1)%nprocesos;
	}

	while(1){
		//Solicita tenedor izquierdo
		cout<<" Filosofo "<<id<< " solicita tenedor izq ..."<<izq <<endl;
		MPI_Ssend(&pet_tenedor, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);

		//Solicita tenedor derecho
		cout<<" Filosofo "<<id<< " solicita tenedor der ..."<<der <<endl;
		MPI_Ssend(&pet_tenedor, 1, MPI_INT, der, 0, MPI_COMM_WORLD);

		//Filosofo Comiendo
		cout<<" Filosofo "<<id<< " COMIENDO"<<endl;
		sleep((rand() % 3)+1); //comiendo

		//suelta el tenedor izquierdo
		cout<<" Filosofo "<<id<< " suelta tenedor izq ..."<<izq <<endl;
		MPI_Ssend(&solicitud_tenedor, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);

		//suelta el tenedor derecho
		cout<<" Filosofo "<<id<< " suelta tenedor der ..."<<der <<endl;
		MPI_Ssend(&solicitud_tenedor, 1, MPI_INT, der, 0, MPI_COMM_WORLD);

		//Filosofo pensando
		cout<<" Filosofo "<<id<< " PENSANDO"<<endl;
		sleep((rand()%3)+1 );//pensando
	}
}

void Tenedor(int id, int nprocesos){
	int buf; MPI_Status status; int Filo;
	while(1){
		// Espera un peticion desde cualquier filosofo vecino ...

		// Recibe la peticion del filosofo ...
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		Filo = status.MPI_SOURCE;
		cout << " Tenedor " << id << " recibe petición de filosofo " << Filo << endl;

		// Espera a que el filosofo suelte el tenedor...
		MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD, &status);
		cout << " Tenedor " << id << " recibe liberación de " << Filo << endl;
	}
}

int main(int argc,char** argv ){
	int rank,size;

	srand(time(0));
	
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	if( size!=10){
		if (rank == 0) cout<<"El numero de procesos debe ser 10"<<endl;
		MPI_Finalize( ); 
		return 0;
	}
	
	if ((rank%2) == 0) 
		Filosofo(rank,size); // Los pares son Filosofos
	else 
		Tenedor(rank,size); // Los impares son Tenedores
	
	MPI_Finalize();
	return 0;
} 
/*
 Filosofo 8 solicita tenedor izq ...7
 Tenedor 1 recibe petición de filosofo 2
 Filosofo 2 solicita tenedor izq ...3
 Filosofo 2 solicita tenedor der ...1
 Filosofo 2 COMIENDO
 Tenedor 3 recibe petición de filosofo 2
 Tenedor 7 recibe petición de filosofo 8
 Filosofo 8 solicita tenedor der ...9
 Filosofo 4 solicita tenedor izq ...3
 Filosofo 0 solicita tenedor izq ...9
 Filosofo 6 solicita tenedor izq ...7
 Tenedor 9 recibe petición de filosofo 8
 Filosofo 8 COMIENDO
 Filosofo 2 suelta tenedor izq ...3
 Tenedor 3 recibe liberación de 2
 Filosofo 2 suelta tenedor der ...1
 Tenedor 3 recibe petición de filosofo 4
 Filosofo 4 solicita tenedor der ...5
 Tenedor 1 recibe liberación de 2
 Filosofo 2 PENSANDO
 Filosofo 4 COMIENDO
 Tenedor 5 recibe petición de filosofo 4
 Filosofo 8 suelta tenedor izq ...7
 Tenedor 7 recibe liberación de 8
 Filosofo 8 suelta tenedor der ...9
 Tenedor 9 recibe liberación de 8
 Filosofo 8 PENSANDO
 Tenedor 9 recibe petición de filosofo 0
 Filosofo 0 solicita tenedor der ...1
 Tenedor 7 recibe petición de filosofo 6
 Filosofo 6 solicita tenedor der ...5
 Filosofo 0 COMIENDO
 Tenedor 1 recibe petición de filosofo 0

*/