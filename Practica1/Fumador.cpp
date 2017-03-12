#include <time.h>   // incluye ”time(....)”
#include <unistd.h> // incluye ”usleep(...)”
#include <stdlib.h> // incluye ”rand(...)” y ”srand”
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

sem_t existe_tabaco;	//Este semáforo controlará si el estanquero ha servido tabaco
sem_t existe_papel;	//Este semáforo controlará si el estanquero ha servido papel
sem_t existe_cerilla;	//Este semáforo controlará si el estanquero ha servido cerilla

sem_t producto_servido;	/*Este semáforo controlará que el estanquero no sirva productos hasta que un fumador no coja los productos servidos*/

string cerilla = "cerilla";
string tabaco = "tabaco";
string papel = "papel";
string Ingredientes[]={cerilla,tabaco,papel};

int aleatorio;

// funcion que simula la accion de fumar
// como un retardo aleatorio de la hebra
void fumar(){ 
    // calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
    const unsigned miliseg = 100U + (rand() % 1900U) ;
    usleep( 1000U*miliseg ); // retraso bloqueado durante miliseg milisegundos
}

void * intentarFumar_sin_cerilla(void *){
    int cigarros_fumados = 0;
    while(true){
        sem_wait(&existe_cerilla);          //Esperamos hasta que el estanquero ponga la cerilla
        cout << "Soy el fumador que necesita cerillas, he fumado "<<cigarros_fumados<<" cigarros, voy a fumar..." << endl;
        sem_post(&producto_servido);        //Dejamos servir un nuevo producto
        fumar();
	cout << "Soy el fumador que necesita cerillas , he acabado de fumar" << endl;
	cigarros_fumados++;
    }
}

void * intentarFumar_sin_papel(void *){
    int cigarros_fumados = 0;
    while(true){
        sem_wait(&existe_papel);            //Esperamos hasta que el estanquero ponga el papel 
        cout << "Soy el fumador que necesita papel, he fumado "<<cigarros_fumados<<" cigarros, voy a fumar..." << endl;
        sem_post(&producto_servido);        //Dejamos servir un nuevo producto
        fumar();
	cout << "Soy el fumador que necesita papel , he acabado de fumar" << endl;
	cigarros_fumados++;
    }
}

void * intentarFumar_sin_tabaco(void *){
    int cigarros_fumados = 0;
    while(true){
        sem_wait(&existe_tabaco);           //Esperamos hasta que el estanquero ponga el tabaco
        cout << "Soy el fumador que necesita tabaco, he fumado "<<cigarros_fumados<<" cigarros, voy a fumar..." << endl;
        sem_post(&producto_servido);        //Dejamos servir un nuevo producto
        fumar();
	cout << "Soy el fumador que necesita tabaco , he acabado de fumar" << endl;
	cigarros_fumados++;
    }
}

/*Fabrica un producto de la lista de productos aleatorio*/
string proporcionarProducto(){
    aleatorio = rand() % 3;
    string producto = Ingredientes[aleatorio];
    return producto;
}

void * servir(void *){
    while(true){
        sem_wait(&producto_servido);                           //Esperamos a que los fumadores cojan el producto
        string producto = proporcionarProducto();
        cout << "\nSoy el estanquero y proporciono:" << producto << "\n" << endl;

        if(producto == cerilla)
            sem_post(&existe_cerilla);                          //Proporciona cerilla
        else if(producto == tabaco)
            sem_post(&existe_tabaco);                           //Proporciona tabaco
        else
            sem_post(&existe_papel);                            //Proporciona papel
    }

}

int main(){
    srand( time(NULL) ); // inicializa la semilla aleatoria

    pthread_t hebra_cerilla_papel, hebra_cerilla_tabaco, hebra_papel_tabaco;
    pthread_t hebra_estanquero;

    sem_init(&existe_papel, 0, 0);          //Inicialmente no hay nada
    sem_init(&existe_tabaco, 0, 0);
    sem_init(&existe_cerilla, 0, 0);

    sem_init(&producto_servido, 0, 1);      //Inicialmente podemos producir

    pthread_create(&hebra_cerilla_papel, NULL, intentarFumar_sin_tabaco, NULL);
    pthread_create(&hebra_cerilla_tabaco, NULL, intentarFumar_sin_papel, NULL);
    pthread_create(&hebra_papel_tabaco, NULL, intentarFumar_sin_cerilla, NULL);

    pthread_create(&hebra_estanquero, NULL, servir, NULL);
 
    pthread_join(hebra_cerilla_papel, NULL);
    pthread_join(hebra_cerilla_tabaco, NULL);
    pthread_join(hebra_papel_tabaco, NULL);

    pthread_join(hebra_estanquero, NULL);

    sem_destroy(&existe_papel);
    sem_destroy(&existe_tabaco);
    sem_destroy(&existe_cerilla);

    sem_destroy(&producto_servido);
}



