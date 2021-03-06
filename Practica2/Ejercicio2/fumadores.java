import monitor.*; 
import java.util.Random ;

class Estanco extends AbstractMonitor
{
	public static final int numF = 3;
	private int productoPuesto = -1;

	private Condition[] productos = new Condition[numF];

	private Condition recogeProducto = makeCondition();
	boolean productoRecogido = false;

  	public Estanco(){
  		for(int i=0; i<numF; i++) {
  			productos[i] = makeCondition();	
  		}
  	}

   	// Cada fumador invoca este metodo con su numero para obtener su ingrediente
   	public void obtenerIngrediente( int miIngrediente ){ 
   		enter();

   		if(miIngrediente != productoPuesto)
   			productos[miIngrediente].await();

 	  	productoRecogido = true;
   		recogeProducto.signal();

   		leave();
	}

	// Indica el ingrediente que ha puesto el estanquero
	public void ponerIngrediente( int ingrediente ){
		enter();

		productoRecogido = false;
		productoPuesto = ingrediente;

		productos[ingrediente].signal();

		leave();	
	}
	// Lo invoca el estanquero
	public void esperarRecogidaIngrediente(){
		enter();

		if(productoRecogido == false)
			recogeProducto.await();

		leave();
	}
}

class aux
{
	static Random genAlea = new Random() ;
	static void dormir_max( int milisecsMax ){ 
		try
		{ 
			Thread.sleep( genAlea.nextInt( milisecsMax ) ) ; 
		}catch( InterruptedException e ){ 
			System.err.println("sleep interumpido en ’aux.dormir_max()’");
		}
	}
}

class Fumador implements Runnable
{
	int miIngrediente;
	public Thread thr ;
	private Estanco estanco;
	
	public Fumador( int p_miIngrediente, Estanco p_estanco ){
		estanco = p_estanco;
		miIngrediente = p_miIngrediente;
		thr 	= new Thread(this, "fumador " + miIngrediente );
	}
	public void run(){
		while ( true ){ 
			estanco.obtenerIngrediente( miIngrediente );
			System.out.println("\n\t Fumador " + miIngrediente + " voy a fumar \n");
			aux.dormir_max( 2000 );
			System.out.println(" \t Fumador " + miIngrediente + " he fumado \n");
		}
	}
}

class Estanquero implements Runnable
{ 
	public Thread thr ;
	private Estanco estanco;

	public Estanquero(Estanco p_estanco){
		estanco = p_estanco;
		thr 	= new Thread(this, "estanquero");
	}
	public void run()
	{ 
		int ingrediente ;
		while (true){
			ingrediente = (int) (Math.random () * 3.0); // 0,1 o 2
			System.out.println("\n Soy el estanquero y sirvo " + ingrediente + " \n");
			estanco.ponerIngrediente( ingrediente );
			estanco.esperarRecogidaIngrediente() ;
		}
		
	}
}

class EjemploFumadores 
{ 
  public static void main( String[] args ) 
  { 
    aux.dormir_max(10);
	Fumador[] fumadores = new Fumador[Estanco.numF];
	
	Estanco estanco = new Estanco();
	for(int i = 0; i < Estanco.numF; i++)
		fumadores[i] = new Fumador(i,estanco);
	Estanquero estanquero = new Estanquero(estanco);

	for(int i = 0; i < Estanco.numF; i++) 
		fumadores[i].thr.start();
	estanquero.thr.start();
  }
}