#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define DIRNUM 4
int hasPassed[DIRNUM];

enum direction{
    west,
    south,
    east,
    north
} ;

char * dirstr[DIRNUM] = { "West", "South", "East", "North"};

struct thread_data{
    int dir;
    int id;
    int order;
};
struct thread_data * dataArray;

pthread_cond_t queueConds[DIRNUM];
pthread_cond_t firstConds[DIRNUM];
pthread_mutex_t queueMutex[DIRNUM];
int queueSize[DIRNUM];
int isCarWaiting[DIRNUM];

pthread_t * threads;
pthread_attr_t attr;

pthread_mutex_t printMutex;
pthread_mutex_t passMutex;


void print_msg(char * format, ...){
    va_list list;
    va_start(list, format);

    pthread_mutex_lock(&printMutex);
    vprintf(format, list);
    pthread_mutex_unlock(&printMutex);
    
    va_end(list);
    return;
}

void debug_msg(int dir){
    int i;
    print_msg("This is %s: ", dirstr[dir]);
    for( i=0; i<DIRNUM; ++i){
        print_msg("%s = %d, ", dirstr[i], queueSize[i]);
    }
    puts("");
}

int check_deadlock(){
// if deadlock possible, i.e., there are cars waiting in all directions 
//  return 1; else return 0    
    int i;
    for( i=0; i<DIRNUM; ++i){
        if( !isCarWaiting[i] )
            return 0; 
    }
    return 1;
}

void * queue_thread(void * data) {
    int dir = ((struct thread_data *)data)->dir;
    int id = ((struct thread_data *)data)->id;
    int order = ((struct thread_data *)data)->order;
    
    // Step 1: Enter the waiting queue
    (&queueMutex[dir]);
    queueSize[dir]++;
    //print_msg("id = %d, hasPassed[dir] = %d, order = %d\n", id, hasPassed[dir], order);
    if ( hasPassed[dir] < order ) {       // if there exist other cars before this car, then wait for it passing
                                                // otherwise, enter the 
/*
    The wait will "atomically" unlock the mutex, allowing others acc    ess to the condition variable (for signalling). 
    Then when the condition variable is signalled or broadcast to, one or more of the threads on the waiting list 
    will be woken up and the mutex will be magically locked again for that thread.
*/
        pthread_cond_wait(&queueConds[dir], &queueMutex[dir]);      // then wait until the first car
                                                                                                    // after invoke this function, the mutex is actually unlocked to 
                                                                                                    //  allow other cars in this dir pass, and the mutex will be relock 
                                                                                                    //  again after other cars invoke cond_signal
    }
    sleep(1);               // the car need some time to drive to the crossing 
    print_msg("car %d from %s arrives at crossing\n", id, dirstr[dir]);
    pthread_mutex_unlock(&queueMutex[dir]);

    // Step2: Pass the crossing
    pthread_mutex_lock(&passMutex);         // since the car need to pass by order ==> there is only one car passing each turn
    // debug_msg(dir);
    if( queueSize[ (dir+1)%4 ] > 0 ){           // if there is a car int the right side 
        isCarWaiting[dir] = 1;
        if( check_deadlock() ){                                                     // if there is a possible deadlock`
            print_msg("DEADLOCK: car jam detected, signalling %s to go\n", dirstr[dir]);    // this car pass first
        }else{                                                                                // if there is no dead lock
            pthread_cond_wait(&firstConds[dir], &passMutex);    //  allow other cars pass and wait for a signal from the car at right side
        }
        isCarWaiting[dir] = 0;
    }
    sleep(1);               // simulate the passing process which needs a piece of time

    // Step3: Exit the crossing
    print_msg("car %d from %s leaving crossing\n", id, dirstr[dir]);
    pthread_cond_signal(&firstConds[ (dir+3)%4 ]);      // wake up the car at left side
    pthread_mutex_unlock(&passMutex);

    pthread_mutex_lock(&queueMutex[dir]);
    queueSize[dir]--;
    hasPassed[dir]++;
    // sleep(1);               // the car behind should arrive later since two cars cannot be too close
    pthread_cond_signal(&queueConds[dir]);
    pthread_mutex_unlock(&queueMutex[dir]);

    pthread_exit(NULL);
}

void init()
{
    int i;
    for( i=0; i<DIRNUM; ++i ){
        pthread_cond_init(&queueConds[i], NULL);
        pthread_cond_init(&firstConds[i], NULL);
        pthread_mutex_init(&queueMutex[i], NULL);
        queueSize[i] = 0;
    }
    pthread_mutex_init(&passMutex, NULL);
    pthread_mutex_init(&printMutex, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

void quit()
{
    int i;
    pthread_attr_destroy(&attr);
    for( i=0; i<DIRNUM; ++i ){
        pthread_cond_destroy(&queueConds[i]);
        pthread_cond_destroy(&firstConds[i]);
        pthread_mutex_destroy(&queueMutex[i]);
    }
    pthread_mutex_destroy(&passMutex);
    pthread_mutex_destroy(&printMutex);

}

int main(int argc, char *argv[])
{
    int i, j, carnum;
    char * arg;
    int tmp[DIRNUM] = {};

    if( argc < 2 ){
        puts("please specify the car sequence.");
        puts("e.g., ./os_exp1-1 nsewwewn");
        exit(0);
    }

    arg = argv[1];
    carnum = strlen(arg);
    threads = (pthread_t *)malloc(sizeof(pthread_t) * carnum);
    dataArray = (struct thread_data *)malloc(sizeof(struct thread_data) * carnum);

    init();
    
    for( i=0; i < carnum; ++i){
        switch( arg[i] ){
            case 'w': j = west; break;
            case 'e': j = east; break;
            case 's': j = south; break;
            case 'n': j = north; break;
            default:printf("unknown direction: %d\n", arg[i]);
                        exit(0);
        }
        dataArray[i].dir = j;
        dataArray[i].id = i+1;
        dataArray[i].order = tmp[j]++;
        pthread_create(&threads[i], &attr, queue_thread, (void *)&dataArray[i]);
    }

    for( i=0; i<carnum; ++i){
        pthread_join(threads[i], NULL);
    }

    quit();

    free(threads);
    free(dataArray);
    pthread_exit(NULL);
}



