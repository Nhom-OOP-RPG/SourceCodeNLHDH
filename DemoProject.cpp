#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include<iostream>
using namespace std;
// Maximum number of chairs
#define MAX_CHAIR 4
// Maximum number of customers
#define MAX_CUSTOMERS 20

// define p, v operation
#define p(x) sem_wait(&x)
#define v(x) sem_post(&x)

// chair
int freeSeats;
int n;
// Barber and customer semaphore

sem_t barber, customers;
// Mutually exclusive semaphore protection critical area (ie chair)
pthread_mutex_t seats;

// Initialize the number of chairs and semaphore
int init()
{
    freeSeats = MAX_CHAIR;
    return (sem_init(&barber, 0, 1) || sem_init(&customers, 0, 0));
}

void * Barber(void *arg){

    cout << "The barber shop opens...\n";
    while (1)
    {
        p(customers); // try to serve a customer, ortherwise sleep
        // print 
        pthread_mutex_lock(&seats);
        ++ freeSeats;
        cout << "The barber is cutting hair for a customer...\n";
        cout << "Number of free seats: " << freeSeats << "\n";
        pthread_mutex_unlock(&seats);
        v(barber);
        cout << "The barber has done one!\n";
        sleep(2);
    }
}

void * Customer(void *arg){

    int * id_p = (int *)arg;
    int id = *id_p ;

    cout << "Customer #" <<  id << " comes ... \n";

    pthread_mutex_lock(&seats);
    if (freeSeats > 0)
    {
        -- freeSeats;
        v(customers); // Add a new customer
        cout << "Number of free seats: " << freeSeats << "\n";
        pthread_mutex_unlock(&seats);
        p(barber);   // Waiting for the barber
        cout << "Customer #" << id << " is ready to be cutted his hair... \n";
        sleep(1);
    }else{
        pthread_mutex_unlock(&seats);
        cout << "Customer #" << id << " left ... \n";
    } 
       
}

void * Start(void *arg){

    cout << "The maximum number of customer is 20\n";
    cout << "The maximum number of free chair is 4\n";
    cout << "====================================\n";
    cout << "Please take the number of customers: ";
    cin >> n;
}

void *End(void *arg){
    cout << "=============== THE END ===============";
}
int main(){ 

    int i;
    if(init()) {
        cout << "initialize semaphore error!\n";
        return 0;
    }

    pthread_t start;
    pthread_t end;
    pthread_t barberID;
    pthread_t customersID[MAX_CUSTOMERS];

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&start, &attr, Start, NULL);
    pthread_join(start, NULL);

    pthread_create(&barberID, &attr, Barber, NULL);

    for (i = 0; i < n; ++i) {
        pthread_create(&customersID[i], &attr, Customer, (void *) &i);
        sleep(1);
    }

    pthread_create(&end, &attr, End, NULL);

    pthread_join(barberID, NULL);

    for (i = 0; i < n; ++i) pthread_join(customersID[i], NULL);

    pthread_join(end, NULL);

    return 0;
}