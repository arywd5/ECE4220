//Allison Yaeger 
//14244528
//Lab2 Week2

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <pthread.h>


#define MY_PRIORITY 51
#define PERIOD1 315000
#define WAIT1 5000
#define WAIT2 10000
#define WAIT3 15000

//structure to pass to the threads 
typedef struct Arguments{
	FILE *fPtr;	//to point to the file we will scan in from 
	char *buffer;	//common buffer to save the new string to  
	int waittime;
	char **song;
}args;

void *read1( args *ptr);	//this function will read in file and save to the common buffer 
void *read2(args *ptr);		//this function reads in the second file and saves to the common buffer 
void *join(args *ptr);		//this function will join the two files and recreate the song 

int main(){
	
	FILE *ffPtr, *lfPtr;
	//open files for reading 	
	ffPtr = fopen("first.txt", "r");
	lfPtr = fopen("second.txt", "r");
	
	//check that files opened properly 
	if(ffPtr == NULL || lfPtr == NULL ){
		printf("\nOne or more files could not be opened please try again.");
		return -1;
	}
	
	args t1, t2, t3;			//structure to send to the different thread functions
	char *buff;				//buffer to pass to the different threads 
	buff = malloc(sizeof(char)*50);
	char **stringArray;
	int i;
	t3.song = (char **)malloc(sizeof(char *) *20);
	for(i = 0; i < 20; i++){			//malloc for size of 20 characters 
		*(t3.song + i) = (char *)malloc(sizeof(char)*50);
	}
		 
	t1.fPtr = ffPtr;			//point all of our arguments to the correct files and buffer
	t1.buffer = buff;
	t1.waittime = WAIT1;
	t1.song = NULL;
	//t2 is for our thread that reads from second.txt
	t2.fPtr = lfPtr;
	t2.buffer = buff;
	t2.waittime = WAIT3;
	t2.song = NULL;
	//t3 is for our thread that stores the buffer into our song variable 
	t3.fPtr = NULL;
	t3.buffer = buff;
	t3.waittime = WAIT2;
	//t3.song = stringArray; 
	
	//create our threads
	pthread_t thread1, thread2, thread3;
	pthread_create(&thread1, NULL, (void *)read1, (void *)&t1);
	pthread_create(&thread3, NULL, (void *)join, (void *)&t3);
	pthread_create(&thread2, NULL, (void *)read1, (void *)&t2);

	//join threads 
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);	
	
	stringArray = t3.song;		
	
	for(i = 0; i < 20; i++){
		printf("\n%s", *(stringArray + i));
	}

	
	//make sure to close files to avoid a memory leak 
	fclose(ffPtr);
	fclose(lfPtr);
	free(buff);	
	for(i = 0; i < 16; i++)
		free(*(t3.song + i));
	free(t3.song);
		
	return 0;
}

void *read1(args *ptr){
	args *data;
	data = (args *)(ptr);
	int i;
	size_t charac;
	size_t size = 50;
	
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec itval;

	//set up start time 
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = data->waittime; 
	
	//set up period 	
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = PERIOD1;
	

	//set the scheduler 
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	//Start timer 
	timerfd_settime(timer, 0, &itval, NULL);
	
	
	while(!feof(data->fPtr)){
		read(timer, &num_periods, sizeof(num_periods));
		charac = getline(&(data->buffer), &size, data->fPtr);
	}		
//	if(num_periods > 1){
//		printf("\nMISSED WINDOW IN READ1");
//		exit(0);
//	}

	pthread_exit(0);
}

void *join(args *ptr){
	args *data;
	data = (args *)(ptr);
	int i = 0, j = 0;
		
	//set the scheduler
        struct sched_param param;
        param.sched_priority = MY_PRIORITY;
        sched_setscheduler(0, SCHED_FIFO, &param);
        uint64_t num_periods = 0;

	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
        struct itimerspec itval;

        //set up start time
        itval.it_value.tv_sec = 0;
        itval.it_value.tv_nsec = data->waittime;

        //set up period
        itval.it_interval.tv_sec = 0;
        itval.it_interval.tv_nsec = (PERIOD1/2);
	
	//Start timer
        timerfd_settime(timer, 0, &itval, NULL);
	
    for(i = 0; i < 16; i++){
    	read(timer, &num_periods, sizeof(num_periods));
  	j = 0;
	while(*(data->buffer + j) != '\0'){
	*(*((data->song) + i) + j) = *(data->buffer + j);
	j++;  
	}
    }
/*	if(num_periods > 1){
		printf("\nMISSED WINDOW IN JOIN SONG");
		exit(0);
	}
*/
	pthread_exit(0);
}
