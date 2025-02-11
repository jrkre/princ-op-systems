#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct _thread_data_t {
    const int *data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;


int readFile(char filename [], int values_array [])
{
    //printf("trying to open: %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("%s\n", "Error: file not found");
        return -1;
    }
    // printf("%s\n", "reading file...");

    int num_vals = 0;
    int i = 0;
    while (!feof(file) && i < 10000000)
    {
        num_vals += fscanf(file, "%d", &values_array[i]);
        i++;
    }

    fclose(file);


    // printf("numvals: %d\n", num_vals);
    // printf("i: %d\n", i);
    return num_vals;
}

void* arraySum(void* void_data)
{

    // printf("%s\n", "arraysum");
    //takes its given (void* thread_data), breaks it down into given indeces, and sums the results into totalSum

    thread_data_t * data = (thread_data_t*) void_data;

    long long int threadSum = 0;

    for (int i = data->startInd; i < data->endInd; i++)
    {
        threadSum += data->data[i];
    }
    
    // printf("%lld start: %d end: %d\n", threadSum, data->startInd, data->endInd);
    pthread_mutex_lock(data->lock);
    *data->totalSum += threadSum;
    pthread_mutex_unlock(data->lock);

    return data->totalSum;
}

int main(int argc, char* argv[])
{
    //argument # check
    if (argc < 3)
    {
        printf("%s\n", "Error: Not enough arguments");
        printf("%s\n", "Usage: ./threaded_sum <filename> <thread_count>");
        return -1;
    }
    else if (argc > 3)
    {
        printf("%s\n", "Error: Too many arguments");
        printf("%s\n", "Usage: ./threaded_sum <filename> <thread_count>");
        return -1;
    }

    int * vals = (int*) malloc(10000000 * sizeof(int));

    char * filename = argv[1];
    
    // printf("%s\n", filename);

    int ints_read = readFile(filename, vals);

    int thread_count = atoi(argv[2]);

    // thread count/value count check
    if (ints_read < thread_count)
    {
        printf("%s\n", "Error: Too many threads requested");
        return -1;
    }

    long long int totalSum = 0;


    struct timeval start_time = (struct timeval) {0};

    if (gettimeofday(&start_time, NULL) != 0)
    {
        printf("%s\n","Error: failed to get time of day");
    }

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_init(&mutex, NULL);

    // make array of pthread_arg_t structs, based on argv[2] and ints_read

    thread_data_t * thread_data_array = (thread_data_t*) malloc(thread_count * sizeof(thread_data_t));

    // numbers per thread
    int ints_per_thread = ints_read / thread_count;

    printf("%s ints per thread: %d\n", "initialize thread_data_array", ints_per_thread);
    
    // construct thread_data_t
    for (int i = 0; i < thread_count; i++)
    {
        // int_per_thr * thread_count == ints_read
        thread_data_array[i].startInd = ints_per_thread * i;
        if (i == thread_count - 1) 
        {
            thread_data_array[i].endInd = ints_read; 
        } 
        else 
        {
            thread_data_array[i].endInd = ints_per_thread * (i + 1);
        }

        thread_data_array[i].data = vals;
        
        thread_data_array[i].lock = &mutex;
        thread_data_array[i].totalSum = &totalSum;

    }

    // make array of pthreads
    pthread_t * threads = malloc(thread_count * sizeof(pthread_t));

    // printf("%s\n", "pthread_create");

    
    // call pthread_create on all of the pthread
    for(int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, arraySum, &thread_data_array[i]);
    }

    // printf("%s\n", "pthread_join");

    // call pthread_join on all of the pthreads
    for(int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // printf("%s\n", "calculate sum and time values");


    totalSum = *thread_data_array[0].totalSum;

    struct timeval end_time = (struct timeval) {0};


    if (gettimeofday(&end_time, NULL) != 0)
    {
        printf("%s\n", "Error: failed to get time of day");
    }


    suseconds_t total_time_ms = (end_time.tv_usec - start_time.tv_usec) * 1000;


    //return/print results
    printf("Total Time: %ld\n", total_time_ms);
    printf("Total Sum: %lld\n", totalSum);

    free(threads);
    free(thread_data_array);

    return 0;

}