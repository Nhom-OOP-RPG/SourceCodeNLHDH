#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
using namespace std;

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
    cout << "The barber shop opens...\n";
    while (1){
        //wait(customers)
        p(customers);
        
        //wait(seat)
        pthread_mutex_lock(&seats);
        
        //freeseats++
        ++ freeSeats;
        
        cout << "\nThe barber is cutting hair for a customer...\n";
        cout << "Number of free seats: " << freeSeats << "\n";
		sleep(rand() % 4 + 1);
		
        //signal(seat)
        pthread_mutex_unlock(&seats);
        
        //signal(barber)
        v(barber);
        
        cout << "The barber has done one!\n";
        if(freeSeats == MAX_CHAIR && i == n) pthread_exit(NULL);
    }
}

void * Customer(void *arg){
    int * id_p = (int *)arg;
    int id = *id_p ;
    
    cout << "\nCustomer #" <<  id << " comes ... \n";
    
    //wait(seat)
    pthread_mutex_lock(&seats);
    
    if (freeSeats > 0){
    	
        //freeseats--
        freeSeats--;
        cout << "Number of free seats: " << freeSeats << "\n";
        
        //signal(customers)
        v(customers);
        
        //signal(seat)
        pthread_mutex_unlock(&seats);
        
        //wait(barber)
        p(barber);
        cout << "Customer #" << id << " is ready to be cutted his hair... \n";
    } else {
        //signal(seat)
        pthread_mutex_unlock(&seats);
        cout << "There is no freeseat left.\n";
        cout << "Customer #" << id << " left ... \n";
    }       
}

int main(){ 
    if(init()) {
        cout << "initialize semaphore error!\n";
        return 0;
    }
    cout << "The maximum number of customer is 20\n";
    cout << "The maximum number of free chair is 4\n";
    cout << "====================================\n";
    cout << "Please take the number of customers: ";
    cin >> n;
    cout << "====================================\n";
    cout << "=============== START ==============\n";
    cout << "====================================\n";

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
