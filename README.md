# SCD
Laboratory projects from the "Sistemas concurrentes y distribuidos" course

1. 	First laboratory project:
	-Resolution of the producer and consumer problem with java semaphores
	There are two solutions, each one with a different management of the intermediate vector: LIFO(Last In First Out) [prod-consLIFO.cpp](/Practica1/prod-consLIFO.cpp) and FIFO(First In First Out) [prod-consFIFO.cpp](/Practica1/prod-consFIFO.cpp)

	-Resolution of the Smokers problem. There is a tobacco Shop wich sell 3 types of products: Tabacco, paper and matches. 	The tabacco shop produce these ingredients one by one, and if anyone buy the product it must wait for sell it before produce another one. Always the clients have 2 ingredients and they must wait for buy their products until the tobacco shop produce them. [Fumador.cpp](/Practica1/Fumador.cpp)

2. Second laboratory project:
	-Resolution of the producer and consumer problem with monitors and a limited buffer. [prodconsum.java](/Practica2/Ejercicio1/prodconsum.java)

	-Smokers Problem with monitors [fumadores.java](/Practica2/Ejercicio2/fumadores.java)

	-Barber shop problem: In a barber shop there are a lot of clients and one barber. If the barber is shaving one client the rest of the clients must wait, and the barber will call the client who has wait for more time. [barbero.java](/Practica2/Ejercicio3/barbero.java)

3. Third laboratory project:
	-Resolution of the producer and consumer problem with limited buffer and MPI (Message Passage Interface). [prodcons.cpp](/Practica3/prodcons.java)

	-Philosophers dinner problem with MPI. [cenaF.cpp](/Practica3/cenaF.cpp)

	-Philosophers problem with another control process. [cenaFC.cpp](/Practica3/cenaFC.cpp)