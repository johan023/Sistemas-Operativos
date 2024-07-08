#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <ucontext.h>
#include <sys/types.h>
#include <time.h>
#include "funciones.h"
extern int contHijos;
extern int n_sorteos;
extern Hijo* DatosHijos;
int comprobarParametros(int argc, char** argv)//Esto es una variable :D
{
		if (argc == 3){
		printf("Se han pasado correctamente los argumentos\n");

		if (atoi(argv[1]) <= 15 && atoi(argv[1]) >= 1){
			printf("Número de sorteos correcto\n");

			if(atoi(argv[2]) <= 10 && atoi(argv[2]) >= 1){
				printf("Número de jugadores correcto\n");
				printf("Todos los parámetros correctos\n"); 			
			}else{
				printf("Número de jugadores incorrecto\n");
				exit(0);
			}

		}else{
			printf("Número de sorteos incorrecto\n");
			exit(0);
		}

	}else{
		printf("No hay dos argumentos\n");
		exit(0);
	}
}					
	
int ComprobarRepetido(int *combinacion, int total, int x){
	int i=0;
		for(i=0; i<total; i++){
			if (combinacion[i]==x){
				return i;
			}
		}
return -1;
}

int CompararNumeros(const void *a, const void *b){
	int *c = (int*)a;
	int *d = (int*)b;

	if(*c < *d){
		return -1;
	}else if(*d < *c){
		return 1;
	}else{
		return 0;
	}
}

//Envía la señal al padre para que sepa que los hijos han apostado

void ApuestaRealizada(){
	union sigval v;
	v.sival_int = getpid();
	sigqueue(getppid(),SIGRTMIN, v);//sigqueue(A quién enviamos la señal | El número de señal | La información enviada de la señal)
}

//Aumenta el valor de la variable global y contea los hijos que han apostado
void HijoApuesta(int sig, siginfo_t *siginfo, void *context){
	contHijos++;
}

void SorteoRealizado(int sig, siginfo_t *siginfo, void *context){
	n_sorteos++;
}
void GenerarCombinacion(int *combinacion){
	srand(time(NULL)+getpid());
	int total = 0;
	int x;
	int repetido;
	while (total < 6){
		x = rand()%15+1;
		repetido = ComprobarRepetido(combinacion, total, x);
		if (repetido == -1){
			combinacion[total]=x;
			total++;
		}
	}
	qsort(combinacion, 6, sizeof(int), CompararNumeros);
}

void CrearTuberia(int numJugadores){
	for (int i=0; i < numJugadores; i++){
		pipe(DatosHijos[i].pipeHijo);
	}
}
void ComunicarApuesta(int numJugadores, int *combinacion){
	union sigval v;
	v.sival_int = getppid();
	sigqueue(DatosHijos[numJugadores].pid, SIGRTMIN+2, v);
	//printf("Soy el padre y envio señal al pid %d y escribo en la tubería %d\n",DatosHijos[numJugadores].pid,DatosHijos[numJugadores].pipeHijo[1]);
	write(DatosHijos[numJugadores].pipeHijo[1], combinacion, 6*sizeof(int));
}

void LeerGanadora(int numJugadores, int *comGanadora){
	read(DatosHijos[numJugadores].pipeHijo[0], comGanadora, 6*sizeof(int));
	//printf("He leído de la tubería %d\n", DatosHijos[numJugadores].pipeHijo[1]);
}

int ComprobarAciertos(int *comGanadora, int *combinacion){
	int i;
	int aciertos = 0;
	int j;	
	for (i=0;i<6;i++){
		for(j=0;j<6;j++){
			if (comGanadora[i]==combinacion[j]){
				aciertos++;
				break;
			}
		}	
	}
	return aciertos;
}

int DeterminarPremio(int aciertos, int i){
	int premio_t = 0;
	if(aciertos == 6){
		premio_t = 10000;
		printf("El hijo %i ha obtenido 6 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=10000;
	}else if(aciertos == 5){
		premio_t = 500;
		printf("El hijo %i ha obtenido 5 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=500;
	}else if(aciertos == 4){
		premio_t = 50;
		printf("El hijo %i ha obtenido 4 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=50;
	}else if(aciertos == 3){
		premio_t = 10;
		printf("El hijo %i ha obtenido 3 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=10;
	}else if(aciertos == 2){
		premio_t = 0;
		printf("El hijo %i ha obtenido 2 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=0;
	}else if(aciertos == 1){
		premio_t = 0;
		printf("El hijo %i ha obtenido 1 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=0;
	}else if(aciertos == 0){
		premio_t = 0;
		printf("El hijo %i ha obtenido 0 aciertos, que tiene un premio de: %i \n", i, premio_t);
		DatosHijos[i].premio=0;
	}
return premio_t;
}

void CrearFichero(int numJugadores){ //n_sorteos es el número actual del sorteo, no el total
	
	int i;
	//Nombre del fichero
	char snsorteo[12]; //12

	sprintf(snsorteo, "S%dR", n_sorteos);
	//printf("numSorteo: %d\n", n_sorteos);
	//printf("Nombre fichero: %s\n", snsorteo);
	FILE *fp;
	fp = fopen(snsorteo, "w");
	fprintf(fp, "#Resultado Sorteo: %i \n", numJugadores);
	for(i=0; i < numJugadores; i++){
	fprintf(fp, "%ld\n", DatosHijos[i].premio); //Guardamos en el fichero
	}
	fclose(fp);
}

