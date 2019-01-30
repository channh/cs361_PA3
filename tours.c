// --------------------------------
// Assignment  : PA02-IPC
// Date        : 10/22/18
//
// Author      : Nathan Chan and Amanda Pearce
// File Name   : tours.c
// ----------------------------------

#include<signal.h>
#include <time.h>
#include "wrappers.h"

#define MAXTOURISTS 50
#define NUMBUSSEATS 3
#define MAXTRIPS    10

// support function that takes the actual number of tourists and it creates all the threads for each tourist, the 0 element will be the driver
void    startThreads(pthread_t thrd[] , long num_tourists );
void    joinThreads(pthread_t thrd[], long num_tourists); /* this  loops through each thread and joins it*/

// two thread functions
void* Indiana (void*);
void* tourist (void *);

// semaphores
sem_t* mutex, *print, *all_arrived, *ready_to_board, *available_seats, *bus_full, *all_seatbelts_on, *touring, *done_singing, *done_touring, *bus_empty, *done_looping;
/* 
mutex: Lock/Unlock for incrementing/decrementing global variables, 
print: a mutex for the output, 
all_arrived: the last tourist will post this when they have arrived,
ready_to_board: Driver posts one for each tourist to let them know the bus is ready to be boarded and the tourists each wait.
available_seats: a semaphore with multiple keys for al the number of seats on the bus.
bus_full: Tourists post this to Driver for when there is no more seats. Alerting Driver to move on. Have a numSeats decrement until 0. Driver will wait on this. 
all_seatbelts_on: Tourist will post this when the last tourist has their seatbelt on 
touring:
done_singing:
bus_empty: the last tourist will post this and Indy waits for it at the end of his loop.
done_looping:
*/
int shopping, tickets, trips, onBoard, in_town, open_seats; /* 
shopping: count of tourists still on the street, vs. those who started a tour on board the bus; 
tickets: Total tickets for today = num Tourists x trips-per-tourist; 
trips: trips each tourist will actually do; 
onBoard: Number of tourists on board the bus 
in_town: a counter of how many toursists are in town.
open_seats: a counter of open seats on the bus. */ 
long num_tourists, seatbelts_buckled; /* Tourist will increment seatbelts when the have buckled there seatbelt. Driver will check if seatbelts equal to NUMBUSSEATS */

unsigned int seed;
/* args num of tourists, how many trips each tourist can take, check if the values are greater than max and stop */
int main(int argc, char * argv[])
{
    pthread_t thrd[MAXTOURISTS+1];
    
    if(argc < 3){
         printf("OPERATOR Usage: %s <num of Tourists up to %d> <num of trips up to %d>", argv[0], MAXTOURISTS, MAXTRIPS);
         exit(-1);
    }
    
    num_tourists = atol( argv[1] ) ;
    trips        = atoi( argv[2] ) ;
    
    if(trips > MAXTRIPS){
        printf("OPERATOR Usage: number of trips is greater than MAXTRIPS, %d > %d.", trips, MAXTRIPS);
        exit(-1);
    }
    if(num_tourists > MAXTOURISTS){
        printf("OPERATOR Usage: number of tourists is greater than MAXTOURISTS, %ld > %d", num_tourists, MAXTOURISTS);
        exit(-1);
    }

    // initializing shared objects
    tickets = num_tourists * trips;
    shopping = 0;
    seatbelts_buckled = 0;
    in_town = 0;
    open_seats = NUMBUSSEATS;
    seed = time(NULL);
    
    int sem_flag, sem_mode;

    sem_flag = O_CREAT;
    sem_mode = S_IRUSR | S_IWUSR;

    mutex           = Sem_open("19mutex_pearceav", sem_flag, sem_mode, 1);
    print           = Sem_open("19print_pearceav", sem_flag, sem_mode, 1);
    all_arrived     = Sem_open("19all_arrived_pearceav", sem_flag, sem_mode, 0);
    ready_to_board  = Sem_open("19ready_to_board_pearceav", sem_flag, sem_mode, 0);
    available_seats = Sem_open("19available_seats_pearceav", sem_flag, sem_mode, 0);
    bus_full        = Sem_open("19bus_full_pearceav", sem_flag, sem_mode, 0);
    all_seatbelts_on = Sem_open("19all_seatbelts_on_pearceav", sem_flag, sem_mode, 0);
    touring         = Sem_open("19touring_pearceav", sem_flag, sem_mode, 0);
    done_singing    = Sem_open("19done_singing_pearceav", sem_flag, sem_mode, 0);
    done_touring    = Sem_open("19done_touring_pearceav", sem_flag, sem_mode, 0);
    bus_empty       = Sem_open("19bus_empty_pearceav", sem_flag, sem_mode, 0);
    done_looping    = Sem_open("19done_looping_pearceav", sem_flag, sem_mode, 0);
    
    printf("**********************************************************************\nOPERATOR\tWe have%3ld tourists for today. Each will make %2d tours\n**********************************************************************\n",num_tourists, trips);

    // Create the Indy and the Tourists threads
    startThreads(thrd, num_tourists);

    //then join then 
    joinThreads(thrd, num_tourists);

    Sem_close(mutex); Sem_unlink("19mutex_pearceav");
    Sem_close(print); Sem_unlink("19print_pearceav");
    Sem_close(all_arrived); Sem_unlink("19all_arrived_pearceav");
    Sem_close(ready_to_board); Sem_unlink("19ready_to_board_pearceav");
    Sem_close(available_seats); Sem_unlink("19available_seats_pearceav");
    Sem_close(bus_full); Sem_unlink("19bus_full_pearceav");
    Sem_close(all_seatbelts_on); Sem_unlink("19all_seatbelts_on_pearceav");
    Sem_close(touring); Sem_unlink("19touring_pearceav");
    Sem_close(done_singing); Sem_unlink("19done_singing_pearceav");
    Sem_close(done_touring); Sem_unlink("19done_touring_pearceav");
    Sem_close(bus_empty); Sem_unlink("19bus_empty_pearceav");
    Sem_close(done_looping); Sem_unlink("19done_looping_pearceav");
    
    printf("\n#### Operator Terminated ####\n");

}

/* This function creates all the threads */
void startThreads(pthread_t thrd[] , long num_tourist ){
    long i ;
    // create the Indiana thread
    Pthread_create( &thrd[0], NULL, Indiana, (void *) num_tourist);
    
    // Create the tourists 
    for( i = 1; i <= num_tourist ; i++)
        Pthread_create( &thrd[i], NULL, tourist, (void *) i);
}

/* This function joins all the threads */
void joinThreads( pthread_t thrd[], long num_tourist){
    // Join all the threads
    for(int i = 0; i <= num_tourist; i++)
        Pthread_join( thrd[i] , NULL);
}

void * Indiana(void *pArg)
{
    long n_tourists = (long) pArg;
    int tours = 0;
                
    Sem_wait(print);
    printf("\nIndy\t : Hey I Just Started\n\n");
    Sem_post(print);

    if(n_tourists != 0){

        Sem_wait(all_arrived);

        while(1){
    
            Sem_wait(mutex);
            if ( in_town == 0 ){
                Sem_post(mutex);
                break;
            }
            Sem_post(mutex);
            int i;
            
            // declare all seats on the bus are now available
            for(i = 0; i < n_tourists; i++)
                Sem_post(ready_to_board);
            
            for(i= 0 ; i < NUMBUSSEATS; i++)
                Sem_post(available_seats);
            
            Sem_wait(print);
            printf("\nIndy\t : New Tour. Declaring %d vacant seats\nIndy\t : Taking a nap till tourists get on board\n", open_seats);
            Sem_post(print);

            Sem_wait(bus_full);

            Sem_wait(print);
    	    printf("Indy\t : Found %d of %ld tourists on board. Welcome! Please buckle your seatbelts.\n", onBoard, n_tourists);
    	    Sem_post(print);

            // wait for all tourists on board to fasten their seatbelts
            Sem_wait(all_seatbelts_on);

            Sem_wait(print);
    	    printf("Indy\t : Thank you for fastening your seatbelts lets go!\n");
    	    Sem_post(print);

            int duration = rand_r(&seed);
            if(duration > 4000) 
                duration = duration % 4000;
            if(duration < 1500)
                duration += 1500;
            
            Sem_wait(print);
    	    printf("Indy\t : Tour will last %d msec\nIndy\t : Bus is now moving. Sing Everyone!\nIndy\t : Bus! Bus! On the street ! Who is the fastest driver to beat?\n", duration);
    	    Sem_post(print);
    	
            // inform tourists that bus has moved
            for(i = 0; i < onBoard; i++)
                Sem_post(touring);
        
            // Simulate driving the tour
            usleep(duration *1000);
        
            // wait for tourists to fininsh their songs
            for(i = 0; i < onBoard; i++)
                Sem_wait(done_singing);

            Sem_wait(print);
    	    printf("Indy\t : Tour is over. Thanks for riding Indiana-Jones coach\n");
    	    Sem_post(print);
    	
            // inform all on board that the tour is over
            for(i = 0; i < onBoard; i++)
                Sem_post(done_touring);
    	    Sem_wait(bus_empty);
    	    tours++;
    	    
    	    // determine whether all tickets are sold out
            if(tickets == 0)
                break;
            else 
                for(i = 0; i < onBoard; i++)
                    Sem_wait(done_looping);
            
        } 
    }
    
    Sem_wait(print);
    printf("\nIndy\t : Business is now closed for the day. I did %d tours today\n", tours);
    Sem_post(print);
    return NULL;
}

void * tourist (void *pArg){

    long my_ID = (long) pArg;
    Sem_wait(print);
    printf("Tourist %ld: Arrived\n", my_ID);
    Sem_post(print);

    Sem_wait(mutex);
    in_town++;
    shopping++;
    if (in_town == num_tourists)
       Sem_post(all_arrived);
    Sem_post(mutex);
    
    for(int my_trips = 0; my_trips < trips; my_trips++)
    {
        //duration is a random integer in the range 500 to 2500 inclusive
        int duration = rand_r(&seed); 
        if(duration > 2500) 
            duration = duration % 2500;
        if(duration < 500)
            duration += 500;
            
        // go shop
        Sem_wait(print);
     	printf("Tourist %ld: Tour # %d. Going to shop for %d msec\n", my_ID, my_trips+1, duration);
     	Sem_post(print);
        usleep(duration * 1000);
        
        Sem_wait(print);
     	printf("Tourist %ld: Back from shopping, waiting for a seat on the bus\n", my_ID);
     	Sem_post(print);

        // determine if bus is ready to be boarded
        Sem_wait(ready_to_board);

        // Wait for an available seat on the bus
        Sem_wait(available_seats);

        // Board bus
        Sem_wait(mutex);
        onBoard++;
        open_seats--;
        shopping--;
        tickets--;
        Sem_post(mutex);
        
        Sem_wait(print);
        printf("Tourist %ld: I got a seat on the bus.\n",my_ID);
        Sem_post(print);

        // notify Indy that bus is full if it is.
      	Sem_wait(mutex);
        if( open_seats == 0 || shopping == 0 )
        	Sem_post(bus_full);
        Sem_post(mutex);

        // Fasten seatbelt and inform Indy
        Sem_wait(print);
     	printf("Tourist %ld: Buckled my seatbelt! #safetyfirst\n", my_ID);
     	Sem_post(print);
     	
     	Sem_wait(mutex);
        seatbelts_buckled++;
     	Sem_post(mutex);

     	Sem_wait(mutex);
        if(seatbelts_buckled == onBoard)
        	Sem_post(all_seatbelts_on);
        Sem_post(mutex);

        Sem_wait(touring);
            
        // Sing and notify Indy when I am done singing
        Sem_wait(print);
     	printf("Tourist %ld: The Wheels on the Bus go Round and round!\n", my_ID);
     	Sem_post(print);

        Sem_post(done_singing);
     	
     	Sem_wait(done_touring);
     	
        Sem_wait(mutex);
        onBoard--;
        open_seats++;
        shopping++;
        seatbelts_buckled--;

        Sem_wait(print);
     	printf("Tourist %ld: I got off the bus\n", my_ID);
     	Sem_post(print);

        if(onBoard == 0){
        	Sem_wait(print);
     		printf("Tourist %ld: Bus is now vacant!\n", my_ID);
     		Sem_post(print);
        	Sem_post(bus_empty);
        }
        Sem_post(mutex);
        Sem_post(done_looping);
    }	
	
	// Leaving town
    Sem_wait(mutex);
    in_town--;
    shopping--;
    Sem_post(mutex);
    Sem_wait(print); 
    printf("Tourist %ld: Leaving Town\n", my_ID);
    Sem_post(print);

    return NULL;
}
