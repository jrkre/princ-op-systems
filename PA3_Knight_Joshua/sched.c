#include <bits/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <bits/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include "print_progress.c"



typedef struct _thread_data_t { 
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;



void* arraySum(void* void_data)
{
    //takes its given (void* thread_data), breaks it down into given indeces, and sums the results into totalSum

    thread_data_t * data = (thread_data_t*) void_data;

    long long int threadSum = 0;

    while(1)
    {
        long latency_max = 0;
        
        for (int i = 0; i < data->numVals; i++)
        {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            long latency = 0;

            threadSum += data->data[i];
            
            clock_gettime(CLOCK_MONOTONIC, &end);
            latency = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
            if (latency > latency_max)
            {
                latency_max = latency;
            }
        }
        
        
        // printf("%lld start: %d end: %d\n", threadSum, data->startInd, data->endInd);
        pthread_mutex_lock(data->lock);
        *data->totalSum += threadSum;
        pthread_mutex_unlock(data->lock);
        
        print_progress(data->localTid, latency_max);
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

    int thread_count = atoi(argv[1]);

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

    pthread_t * threads = malloc(thread_count * sizeof(pthread_t));

    for (int i = 0; i < thread_count; i++)
    {
        thread_array[i].numVals = numVals;
        pthread_create(&threads[i], NULL, arraySum, &thread_array[i]);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);

    free(vals);
    free(thread_array);
    free(threads);

    return 0;
};


