// *****************************************************************************
//
// Practica 1: Productor-consumidor con buffer intermedio.
// Fernando Roldán Zafra 
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std ;
sem_t mutex;			    //inicializado a 1 
sem_t existe_hueco;		//inicializado a 1
sem_t existe_dato;		//inicializado a 0

// ---------------------------------------------------------------------
// constantes configurables:

const unsigned
  num_items  = 40 ,    // numero total de items que se producen o consumen
  tam_vector = 10 ;    // tamaño del vector, debe ser menor que el número de items
  int vector_compartido[tam_vector];	// Vector que comparten la funcion productor y consumidor
  int primera_ocupada = 0;
  int primera_libre = 0; 

// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ---------------------------------------------------------------------
// función que simula la producción de un dato

unsigned producir_dato()
{
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio( 0.1, 0.5 );
  cout << "Productor : dato producido: " << contador << endl << flush ;
  return contador ;
}
// ---------------------------------------------------------------------
// función que simula la consumición de un dato

void consumir_dato( int dato )
{
   retraso_aleatorio( 0.1, 1.5 );
   cout << "Consumidor:                              dato consumido: " << dato << endl << flush ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del productor

void * funcion_productor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    int dato = producir_dato() ;
    sem_wait(&existe_hueco);
    vector_compartido[primera_libre % tam_vector] = dato;
    sem_wait (&mutex);    	
    cout << "Produzco: " << dato << endl << flush ;
  	sem_post (&mutex);
  	primera_libre++;		     	//Variable que almacena la direccion en vector donde sera almacenado el siguiente dato
  	sem_post (&existe_dato);
  }
  return NULL ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor

void * funcion_consumidor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    int dato;
    sem_wait (&existe_dato);
    dato = vector_compartido[primera_ocupada%tam_vector];
    consumir_dato(dato);
    primera_ocupada++; 			//Variable que almacena la direccion en vector del siguiente dato que sera consumido
    sem_wait (&mutex);   
    cout << "Consumo: " << dato << endl << flush ;
    sem_post (&mutex);
    sem_post(&existe_hueco);
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{

  pthread_t hebra_escritora, hebra_lectora;

  sem_init(&mutex,0,1);

  sem_init(&existe_dato,0,0); 			//Inicialmente no hay datos, por lo que no se puede leer

  sem_init(&existe_hueco,0,tam_vector); //Se puede escribir desde el principio ya que el vector esta vacio

  pthread_create(&hebra_escritora, NULL, funcion_productor,NULL);

  pthread_create(&hebra_lectora, NULL, funcion_consumidor, NULL);

  pthread_join(hebra_escritora, NULL);

  pthread_join(hebra_lectora, NULL);
  
  sem_destroy(&existe_hueco);

  sem_destroy(&existe_dato);

  cout << "----------------------------------FIN----------------------------------" << endl;
  
  return 0 ;
}
