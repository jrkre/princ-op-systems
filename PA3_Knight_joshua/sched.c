#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>

typedef struct _thread_data_t { 
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;


void* arraySum(void* void_data)
{

    // printf("%s\n", "arraysum");
    //takes its given (void* thread_data), breaks it down into given indeces, and sums the results into totalSum

    thread_data_t * data = (thread_data_t*) void_data;

    long long int threadSum = 0;

    while(1)
    {
        for (int i = 0; i < data->numVals; i++)
        {
            threadSum += data->data[i];
        }

        // printf("%lld start: %d end: %d\n", threadSum, data->startInd, data->endInd);
        pthread_mutex_lock(data->lock);
        *data->totalSum += threadSum;
        pthread_mutex_unlock(data->lock);
    }
    

    return data->totalSum;
}


int main(int argc, char* argv[])
{

    //argument count check
    if (argc < 2)
    {
        printf("%s\n", "Error: Not enough arguments");
        printf("%s\n", "Usage: ./sched <thread_count>");
        return -1;
    }
    else if (argc > 2)
    {
        printf("%s\n", "Error: Too many arguments");
        printf("%s\n", "Usage: ./sched <thread_count>");
        return -1;
    }

    int thread_count = argv[1];
    printf("%s%d\n", "Thread Count: ", thread_count);

    int numVals = 2000000;
    int * vals = (int*) malloc(numVals * sizeof(int));
    long long int totalSum = 0;

    pthread_mutex_t mutex;

    pthread_mutex_init(&mutex, NULL);

    //initialize thread data array
    thread_data_t * thread_array = (thread_data_t*) malloc(thread_count * sizeof(thread_data_t));

    for (int i = 0; i < thread_count; i ++)
    {
        thread_array[i].data = vals;
        thread_array[i].localTid = i;
        thread_array[i].lock = &mutex;
        thread_array[i].totalSum = &totalSum;
    }

    




};