#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Maximum number of chairs
#define MAX_CHAIR 3
// Maximum number of customers
#define MAX_CUSTOMERS 20

// define p, v operation
#define p(x) sem_wait(&x)
#define v(x) sem_post(&x)

// chair
int freeSeats;
int n, i;
// Barber and customer semaphore
sem_t barber, customers;

// mutex seats
pthread_mutex_t seats;

// Initialize the number of chairs and semaphore
int init(){
    freeSeats = MAX_CHAIR;
    return (sem_init(&barber, 0, 1) || sem_init(&customers, 0, 0));
}

void * Barber(void *arg){
    printf("\nThe barber shop opens...\n");
    while (1){

        p(customers);   //wait(customers)
        pthread_mutex_lock(&seats);    //wait(seat)
        ++ freeSeats;
        printf("\nNumber of free seats: %d\n", freeSeats);
		sleep(rand() % 5 + 1);
		v(barber);	//signal(barber)
		pthread_mutex_unlock(&seats);   //signal(seat)     
        printf( "\nThe barber has done one!\n");
        if(freeSeats == MAX_CHAIR && i == n) pthread_exit(NULL);
    }
}

void * Customer(void *arg){
    int * id_p = (int *)arg;
    int id = *id_p ;
    printf("\nCustomer #%d comes ... \n", id);
    pthread_mutex_lock(&seats);  //wait(seat)  
    if (freeSeats > 0){

        freeSeats--;
        printf("\nNumber of free seats: %d\n", freeSeats);
        v(customers);   //signal(customers)
        pthread_mutex_unlock(&seats);   //signal(seat)
        p(barber);  //wait(barber)
        printf("\nThe barber is cutting hair for #%d...\n", id);
    } else {
        
        pthread_mutex_unlock(&seats);   //signal(seat)
        printf("\nThere is no freeseat left.\n");
        printf("Customer #%d left ... \n", id);
    }       
}

int main(){ 
    if(init()) {
        printf("initialize semaphore error!\n") ;
        return 0;
    }

    printf ("The maximum number of customer is 20\n");
    printf ("The maximum number of free chair is %d\n", MAX_CHAIR);
    printf ("====================================\n");
    printf ("\nPlease take the number of customers: ");
    scanf("%d", &n);
    printf("\n====================================\n");
    printf ("=============== START ==============\n");
    printf ("====================================\n");

    pthread_t barberID;
    pthread_t customersID[MAX_CUSTOMERS];

    pthread_create(&barberID, NULL, Barber, NULL);

    for (i = 0; i < n; i++) {
    	sleep(rand() % 3);
        pthread_create(&customersID[i], NULL, Customer, (void *) &i);
        sleep(1);
    }	

    pthread_join(barberID, NULL);
    for (i = 0; i < n; ++i) pthread_join(customersID[i], NULL);

    printf("============ THE END =============");
    return 0;
}