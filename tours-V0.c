#include <signal.h>
#include <time.h>

#include "wrappers.h"

#define MAXTOURISTS 50
#define NUMBUSSEATS 3
#define MAXTRIPS 10

// create threads from array of threads with number of tourists
// int num = 0 is the driver in thrd[].
void startThreads( pthread_t thrd[], long numTourists );
// join threads from array of threads with number of tourists
void joinThreads( pthread_t thrd[], long numTourists );

void* Indiana( void* );
void* tourist( void* );

int trips; // Number of trips each tourist does
int main (int argc, char * argv[]) 
{
    int i;
    long numTourists ; // Number of tourists in town
    pthread_t thrd[MAXTOURISTS+1];

    if (argc < 3)
    {
        printf("OPERATOR Usage: %s <num of Tourists upto %d> <num of trips %d>", argv[0], MAXTOURISTS, MAXTRIPS);
        exit(-1);
    }

    numTourists = atoi(argv[1]);
    trips = atoi(argv[2]);
    
    startThreads(thrd, numTourists);
    joinThreads(thrd, numTourists);
}

void startThreads(pthread_t thrd[], long numTourists)
{
    long i;
    // Create driver
    Pthread_create(&thrd[0], NULL, Indiana, (void*) numTourists);

    for (i=1; i <= numTourists; i++)
    {
        // Create tourists.
        Pthread_create(&thrd[i], NULL, tourist, (void*) i);
    }
}

void joinThreads(pthread_t thrd[], long numTourists)
{
    long i;
    
    for (i=0; i<= numTourists; i++)
    {
        Pthread_join(thrd[i], NULL);
    }
}

void* Indiana( void *pArg )
{
    long nTourists = (long) pArg;

    printf("Indy    : Hey I Just Started\n");
    printf("Indy    : Found -- of %ld passengers already on board\n", nTourists);

    printf("Indy : Business is now closed for the day\n");
    return NULL;
}
void* tourist( void *pArg)
{
    long myID = (long) pArg;

    printf("Tourists: %ld: Hey I just arrived to Harrisonburg\n", myID);
    printf("Tourists: %ld: leaving Town\n", myID);

    return NULL;
}
