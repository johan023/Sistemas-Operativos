#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
typedef struct {
	int pid;
	int num;
	long premio;
	int numAciertos;
	int pipeHijo[2];
}Hijo;

int comprobarParametros(int argc, char** argv);

int ComprobarRepetido(int *combinacion, int total, int x);

int CompararNumeros(const void *a, const void *b);

void ApuestaRealizada();

void HijoApuesta(int sig, siginfo_t *siginfo, void *context);

void SorteoRealizado(int sig, siginfo_t *siginfo, void *context);

void GenerarCombinacion(int *combinacion);

void CrearTuberia(int numJugadores);

void ComunicarApuesta(int numJugadores, int *combinacion);

void LeerGanadora(int numJugadores, int *comGanadora);

int DeterminarPremio(int aciertos, int i);

int ComprobarAciertos(int *shmArray, int *combinacion);

void CrearFichero(int n_sorteos);
