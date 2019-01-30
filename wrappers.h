#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/wait.h>   // wait(), waitpid()
#include <pthread.h>
// --------------------------------
// Assignment  : PA03-Threads
// Date        : 10/15/18
//
// Author      : Nathan Chan and Amanda Pearce
// File Name   : wrappers.h
// ----------------------------------

#ifndef __WRAPPERS_H__

#define SHMEM_SIZE sizeof(shmData)
#define STR_SIZE 100

/************************************************
 * Wrappers for system call functions
 ************************************************/

pid_t   Fork(void);
void    unix_error(char *msg) ;
int     Shmget(key_t key, size_t size, int shmflg);
void   *Shmat(int shmid, const void *shmaddr, int shmflg);
int     Shmdt( const void *shmaddr ) ;

sem_t  *Sem_open( const char *name, int oflag, mode_t mode, unsigned int value );
sem_t  *Sem_open2( const char *name, int oflag ) ; // what is this?
// Mandy changed these to void since they didn't need to return anything.
int     Sem_close( sem_t *sem );
int     Sem_unlink( const char *name );

int     Sem_wait( sem_t *sem ) ;
int     Sem_post( sem_t *sem ) ;
int     Sem_init( sem_t *sem, int pshared, unsigned int value ) ;
int     Sem_destroy( sem_t *sem ) ;

int     Msgget( key_t key, int msgflg );
ssize_t	Msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg);
int 	Msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflag);

void    Pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
void    Pthread_join(pthread_t thread, void **retval);
#endif 

