#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Maximum number of chairs
#define MAX_CHAIR 5
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
    printf("The barber shop opens...\n");
    while (1){

        p(customers);   //wait(customers)
        pthread_mutex_lock(&seats);    //wait(seat)
        ++ freeSeats;
        printf("\nThe barber is cutting hair for a customer...\n");
        printf("Number of free seats: %d\n", freeSeats);
		sleep(rand() % 4 + 1);
		pthread_mutex_unlock(&seats);   //signal(seat)
        v(barber);     //signal(barber)
        printf( "The barber has done one!\n");
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
        printf("Number of free seats: %d\n", freeSeats);
        v(customers);   //signal(customers)
        pthread_mutex_unlock(&seats);   //signal(seat)
        p(barber);  //wait(barber)
        printf("Customer #%d is ready to be cutted his hair... \n", id);
    } else {
        
        pthread_mutex_unlock(&seats);   //signal(seat)
        printf("There is no freeseat left.\n");
        printf("Customer #%d left ... \n", id);
    }       
}

int main(){ 

    if(init()) {
        printf("initialize semaphore error!\n") ;
        return 0;
    }

    printf ("The maximum number of customer is 20\n");
    printf ("The maximum number of free chair is 4\n");
    printf ("====================================\n");
    printf ("Please take the number of customers: ");
    scanf("%d", &n);
    printf("====================================\n");
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