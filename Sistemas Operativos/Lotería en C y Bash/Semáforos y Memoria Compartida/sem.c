#include <sys/types.h> 
#include <sys/ipc.h>   
#include <sys/sem.h>   
#include <sys/shm.h>  

#define PERMISOS 0644
 
int sGet(int seed)
{
	key_t semKey = ftok("/bin/ls", seed);
	int semID = semget(semKey,1,IPC_CREAT | PERMISOS);
	return semID;
}
 
void sSet(int semID, int value)
{
	semctl(semID, 0, SETVAL, value);
}
 
int sCreate(int seed, int value)
{
	int semID = sGet(seed);
	sSet(semID, value);
	return semID;
}
 
void sWait(int semID)
{
	struct sembuf op_Wait [] = 	{ 0, -1, 0 }; // Decrementa en 1 el semáforo 
	semop ( semID, op_Wait, 1 );
}
 
void sSignal(int semID)
{
	struct sembuf op_Signal [] = { 0, 1, 0 };	// Incrementa en 1 el semáforo 
	semop ( semID, op_Signal, 1 );
}
