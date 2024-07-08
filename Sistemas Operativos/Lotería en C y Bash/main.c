#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "funciones.h"

//Sorteos entre 1 y 15
//Jugadores entre 1 al 10

int contHijos = 0;
int n_sorteos;
Hijo* DatosHijos;

int main(int argc, char** argv){
struct sigaction fatherhandler; //Handler Padre

fatherhandler.sa_sigaction = HijoApuesta;
fatherhandler.sa_flags = SA_SIGINFO;
sigaction(SIGRTMIN, &fatherhandler, NULL);

struct sigaction handler; //Handler Hijo

handler.sa_sigaction = SorteoRealizado;
handler.sa_flags = SA_SIGINFO;

comprobarParametros(argc, argv);
int i;
int j;
int soyPadre = 1;
int numSorteos = atoi(argv[1]);
int numJugadores = atoi(argv[2]);
DatosHijos = (Hijo*)malloc(numJugadores * sizeof(Hijo));
int* combinacion = (int*)malloc(6*(sizeof (int)));
int* comGanadora = malloc(6*(sizeof (int))); //Vector de la combinación ganadora del padre
n_sorteos = atoi(argv[1]);
pid_t pid;

for (i=0;i < numJugadores;i++){
	DatosHijos[i].pid = 0;
	DatosHijos[i].num = 0;
	DatosHijos[i].premio = 0;
}
CrearTuberia(numJugadores);

for(i=0;i< numJugadores;i++){
			
	if(soyPadre == 1){
		pid = fork();
		
	}
	if(pid==0){
		soyPadre = 0;
		printf("Soy el hijo número: %i con PID: %d\n",i, getpid());
		close(DatosHijos[i].pipeHijo[1]);
		sigaction(SIGRTMIN+2, &handler, NULL);
		GenerarCombinacion(combinacion);
		ApuestaRealizada();

		printf("Combinación para el hijo %i: ",i);
		for (j=0;j < 6;j++) {
			printf("%d ", combinacion[j]);
		}
		printf("\n");

		while(n_sorteos < 1){
			pause();
		}
		LeerGanadora(i, comGanadora);
		int aciertos = ComprobarAciertos(comGanadora, combinacion);
		printf("Número de aciertos para el hijo %i: %d \n", i, aciertos);
		exit(aciertos);
	}else{
		DatosHijos[i].pid = pid;
		printf("Soy el padre con PID: %d y he creado al hijo %i \n",getpid(),i);
	}
}

for(i=0;i<numJugadores;i++){
	close(DatosHijos[i].pipeHijo[0]);
}
while (contHijos < numJugadores){
	pause();
}

printf("la combinación ganadora generada por el padre es: ");
GenerarCombinacion(comGanadora);
for (j=0;j < 6;j++) {
	printf("%d ", comGanadora[j]);
}
printf("\n");


	for(i=0; i < numJugadores; i++){
		ComunicarApuesta(i, comGanadora);
	}
int status;
pid_t p;
	for(i=0; i < numJugadores; i++){
		p=wait(&status);
		for(int k=0; k <numJugadores; k++){
			if(DatosHijos[k].pid == p){
				if(WIFEXITED(status)){
					int es = WEXITSTATUS(status);
					DatosHijos[k].numAciertos = es;
					close(DatosHijos[i].pipeHijo[0]);
					close(DatosHijos[i].pipeHijo[1]);
					int premio_t = DeterminarPremio(es, k);
				}
			}
		}
	}
CrearFichero(numJugadores);
printf("Final\n");
return 0;
}

