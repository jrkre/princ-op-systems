#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _thread_data_t {
    const int *data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int readFile(char[], int[])
{

}

void* arraySum(void*)
{
    
}

int main(int argc, char* argv[])
{



}