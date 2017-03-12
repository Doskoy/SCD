import monitor.* ;
import java.util.Random ;

class Barberia extends AbstractMonitor 
{ 
  private boolean yaTermino = false;
  private Condition barberoDurmiendo = makeCondition();   
  private Condition esperaAtencion = makeCondition();
  private Condition finalPelado = makeCondition();


  public void cortarPelo(int numCliente) 
  { 
    enter();   

    System.out.println("\n Entro el cliente " + numCliente);

    if(!barberoDurmiendo.isEmpty())
      barberoDurmiendo.signal();    // pasa a pelarse directamente
    else
      esperaAtencion.await();         // hay mas clientes

    System.out.println(" ----- Pelandose " + numCliente);

    if(yaTermino==false) 
    	finalPelado.await();
    yaTermino = false;

    System.out.println(" Sale " + numCliente);
    
    leave(); 
  }

  public void siguienteCliente() 
  { 
    enter(); 

    if (esperaAtencion.isEmpty()){                          //no hay nadie esperando ==> se duerme
      System.out.println("\n Durmiendo ");
      barberoDurmiendo.await();
      System.out.println("\t¡ Despierta !");
    }
    else
      esperaAtencion.signal();

    leave(); 
  }

  public void finCliente() 
  { 
    enter(); 

    yaTermino = true;
    finalPelado.signal();
    
    leave(); 
  }
}

class aux
{
  static Random genAlea = new Random() ;
  static void dormir_max( int milisecsMax )
  { 
    try
    { 
      Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
    } 
    catch( InterruptedException e )
    { 
      System.err.println("sleep interumpido en 'aux.dormir_max()'");
    }
  }
}

class Cliente implements Runnable 
{ 
 	int numCliente;
  private Barberia barberia;
	public Thread thr;

  public Cliente( Barberia p_barberia, int p_numCliente ) 
  { 
  	numCliente = p_numCliente;
    barberia = p_barberia;
    thr   = new Thread(this,"cliente "+numCliente);
  }

	public void run(){ 
		while (true){ 
			barberia.cortarPelo (numCliente);
   	    	aux.dormir_max( 2000 ); 
   	     
		}
	}
}

class Barbero implements Runnable 
{ 
	private Barberia barberia;
  	public Thread thr;

	public Barbero( Barberia p_barberia ){ 
    	barberia = p_barberia;
    	thr   = new Thread(this,"barbero");
  	}

  	public void run() { 
		while (true){ 
			barberia.siguienteCliente ();
        	aux.dormir_max( 2500 );  
        	barberia.finCliente ();
		}
  	}
}

class EjemploBarberia
{ 
  public static void main( String[] args ) 
  { 
    int numClientes = (int) (Math.random () * 4.0) + 3; // 2-6 clientes

    System.out.println("Barberia con " + numClientes + " clientes.");

	Cliente[] clientes = new Cliente[numClientes];
	Barberia barberia = new Barberia();
		
	for(int i = 0; i < numClientes; i++)
		clientes[i] = new Cliente(barberia,i);
	Barbero barbero = new Barbero(barberia);

	barbero.thr.start();
    for(int i = 0; i < numClientes; i++)
		clientes[i].thr.start();
	  
  }
}
