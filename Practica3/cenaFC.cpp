#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define Tag_Sentarse	0
#define Tag_Tenedor 	1
#define Tag_Levantarse	2

using namespace std;

/* En este problema los filosofos antes de disponerse a pedir los tenedores, el camarero ha de darles
** permiso para sentarse, de esta manera el camarero se ocupa de que no haya un numero de filosofos en
** el que se pueda producir interbloqueo */
void Filosofo(int id, int nprocesos){
	int izq;
	int der;

	int camarero = 10;

	int mensaje = 0;	// Como en mis anteriores programas no he usado el contenido del mensaje para nada, en este caso voy a usar el mismo para todos

	izq=(id+1)%nprocesos;
	der=(id-1+nprocesos)%nprocesos;

	while(1){
		//Solicita sentarse
		cout << " Filosofo "<<id<<" quiere sentarse ..."<<endl;
		MPI_Ssend(&mensaje, 1, MPI_INT, camarero, Tag_Sentarse, MPI_COMM_WORLD);

		//Solicita tenedor izquierdo
		cout<<" Filosofo "<<id<< " solicita tenedor izq ..."<<izq <<endl;
		MPI_Ssend(&mensaje, 1, MPI_INT, izq, 1, MPI_COMM_WORLD);

		//Solicita tenedor derecho
		cout<<" Filosofo "<<id<< " solicita tenedor der ..."<<der <<endl;
		MPI_Ssend(&mensaje, 1, MPI_INT, der, 1, MPI_COMM_WORLD);

		//Filosofo Comiendo
		cout<<" Filosofo "<<id<< " COMIENDO"<<endl<<flush;
		sleep((rand() % 3)+1); //comiendo

		//suelta el tenedor izquierdo
		cout<<" Filosofo "<<id<< " suelta tenedor izq ..."<<izq <<endl;
		MPI_Ssend(&mensaje, 1, MPI_INT, izq, 1, MPI_COMM_WORLD);

		//suelta el tenedor derecho
		cout<<" Filosofo "<<id<< " suelta tenedor der ..."<<der <<endl;
		MPI_Ssend(&mensaje, 1, MPI_INT, der, 1, MPI_COMM_WORLD);

		//Solicita levantarse
		cout << " Filosofo "<<id<<" quiere levantarse ... "<< endl;
		MPI_Ssend(&mensaje,1,MPI_INT,camarero,Tag_Levantarse,MPI_COMM_WORLD);

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
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		Filo = status.MPI_SOURCE;
		cout << " Tenedor " << id << " recibe petición de " << Filo << endl;

		// Espera a que el filosofo suelte el tenedor...
		MPI_Recv(&buf, 1, MPI_INT, Filo, 1, MPI_COMM_WORLD, &status);
		cout << " Tenedor " << id << " recibe liberación de " << Filo << endl;
	}
}
/* El camarero se ocupa de prevenir el interbloqueo de los filosofos, para ello basta que 
** el camarero controle que el numero de filosofos sea menor que el total de sitios en la mesa*/
void Camarero(int id){
	int buf;
	MPI_Status status;
	int Filo;
	int comiendo;

	while(1){
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		Filo = status.MPI_SOURCE;
		if(status.MPI_TAG == Tag_Sentarse){
			comiendo++;
			cout << " Camarero recive peticion de " << Filo << " para sentarse" << endl;
		}
		else if(status.MPI_TAG == Tag_Levantarse){
			comiendo--;
			cout << " Camarero recive peticion de " << Filo << " para levantarse" << endl;
		}
		
		if(comiendo >= 4){
			cout << "Camarero, los 4 sitios estan llenos, no se sienta nadie" << endl;
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, Tag_Levantarse, MPI_COMM_WORLD, &status);
			cout << "  Camarero, ya se puede sentar alguien" << endl;
			comiendo--;
		}	
	}
}

int main(int argc,char** argv ){
	int rank,size;

	srand(time(0));
	
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	if( size!=11){
		if (rank == 0) cout<<"El numero de procesos debe ser 11"<<endl;
		MPI_Finalize( ); 
		return 0;
	}
	if(rank == 10)
		Camarero(rank);
	else if ((rank%2) == 0) 
		Filosofo(rank,size-1); // Los pares son Filosofos
	else
		Tenedor(rank,size-1); // Los impares son Tenedores
	
	MPI_Finalize();
	return 0;
} 
/*
Filosofo 2 quiere sentarse ...
 Filosofo 8 quiere sentarse ...
 Filosofo 4 quiere sentarse ...
 Filosofo 6 quiere sentarse ...
 Filosofo 0 quiere sentarse ...
 Filosofo 8 solicita tenedor izq ...9
 Camarero recive peticion de 8 para sentarse
 Tenedor 9 recibe petición de 8
 Filosofo 8 solicita tenedor der ...7
 Filosofo 8 COMIENDO
 Tenedor 7 recibe petición de 8
Camarero, los 4 sitios estan llenos, no se sienta nadie
 Filosofo 8 suelta tenedor izq ...9
 Tenedor 9 recibe liberación de 8
 Filosofo 8 suelta tenedor der ...7
 Filosofo 8 quiere levantarse ... 
 Tenedor 7 recibe liberación de 8
  Camarero, ya se puede sentar alguien
 Filosofo 8 PENSANDO
 Camarero recive peticion de 0 para sentarse
 Filosofo 0 solicita tenedor izq ...1
Camarero, los 4 sitios estan llenos, no se sienta nadie
 Tenedor 1 recibe petición de 0
 Filosofo 0 solicita tenedor der ...9
 Tenedor 9 recibe petición de 0
 Filosofo 0 COMIENDO
 Filosofo 0 suelta tenedor izq ...1
*/