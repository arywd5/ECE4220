//Allison Yaeger
//14244528

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <wiringPi.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <semaphore.h>
#include <sys/types.h>
#include "ece4220lab3.h"


#define BUTTON 16
#define PERIOD 75000000
#define PRIORITY 55

typedef struct{
	int *pipe;
	int *flag;
	int *gps;
	struct timespec *t;
} data;

void *thread0(void *num);                       //thread function to print values to the screen after reading from N
void process2(int pipe);                     //process to complete the second part of this lab
void *buttonThread(void *ptr);                  //thread to check if the button has been pressed each 75 ms



int main(){

	 wiringPiSetupGpio();                                                    //set up wiring Pi
        pinMode(BUTTON, INPUT);                                                 //declare our button as an input
        pullUpDnControl(BUTTON, PUD_DOWN);

	//declare variables
	int N_pipe1, N_pipe2, number;
	int dummy, f;
	pthread_t t0;
	struct timespec gpsT;
	data *buffer;								//declare buffer 
	buffer = (data *)malloc(sizeof(data *));					//malloc space for the buffer 
	buffer->gps = &number;							//set pointers equal to variables 
	buffer->t = &gpsT;
	buffer->flag = &f;
	buffer->pipe = &N_pipe2;
	
	if((dummy = fork()) < 0){                                               //fork the program and
                printf("\nError Forking lab4.c");                               //check for successful operation
                exit(-1);
        }

        if(dummy == 0){                                                         //for process 2 use the child
                printf("\nForked calling process 2");
		process2(N_pipe2);
        }
	else if (dummy != 0){
		pthread_create(&t0, NULL, (void *)thread0, (void *)buffer);
		if((N_pipe1 = open("/tmp/N_pipe1", O_RDONLY)) < 0){                     //open pipe
                        printf("N_pipe1 error\n");                                      //if the pipe did not open properly exit
                        exit(-1);
                }

                while(1){                                                               //while loop to read in data from the GPS_device
                        if(read(N_pipe1, &number, sizeof(number)) < 0){         //read in pipe and store to buffer
                                printf("\nN_pipe1 reading error\n");
                                exit(-1);
                        }
			else{	
				clock_gettime(CLOCK_MONOTONIC, &gpsT);	
				f = 1;
				usleep(1000);
			}
		}
	}
	 pthread_join(t0, NULL);

	return 0;
}

void *thread0(void *num){
	data *buffer = (data *)num;
	int prev = *(((data *)num)->gps);
	int *f = ((data *)num)->flag;	
	struct timespec hittime;
	printf("\ndata : %d and flag = %d", prev, *f);

	int N_pipe2;
	N_pipe2 = open("N_pipe2", O_RDONLY);
	if(N_pipe2 < 0){
		printf("\nN_pipe 2 could not be opened please tru again");
		exit(0);
	}
	while(1){		
		if(read(N_pipe2, &(hittime), sizeof(hittime)) != sizeof(hittime)){
                      printf("\nError Reading in from N_pipe2 in thread0");
                      exit(-1);	
		}
		else{
//			printf("\n\n%p -> %d at time %ld", ((data *)num)->gps, *(((data *)num)->gps), (*((data *)num)->t).tv_nsec);
//			printf("\nHit time is %ld", hittime.tv_nsec);

		}
	}		
	pthread_exit(0);
}
//second process that will check if the buton has been pressed
void process2(int pipe){

        pthread_t t1;                                                           //create thread
        pthread_create(&t1, NULL, (void *)buttonThread, (void *)&pipe);       //to check button periodically
        pthread_join(t1, NULL);                                                 //join thread

}
//thread function to check wether the button has been pressed
void *buttonThread(void *ptr){
        printf("\nEntering buton thread...");
        struct timespec c1;                                                     //create a clock to get time with
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);                         //create a timer
        int N_pipe2 = open("N_pipe2", O_WRONLY);
	struct itimerspec itval;

        itval.it_value.tv_sec = 0;                                              //set the start time of the timer
        itval.it_value.tv_nsec = 1000;
        itval.it_interval.tv_nsec = 0;                                          //set the period of our timer
        itval.it_interval.tv_nsec = PERIOD;

        struct sched_param param;                                               //set up scheduler
        param.sched_priority = PRIORITY;
        sched_setscheduler(0, SCHED_FIFO, &param);
        uint64_t num_periods = 0;

        timerfd_settime(timer, 0, &itval, NULL);                                //set timer and read first value
      read(timer, &num_periods, sizeof(num_periods));
        
	while(1){                                                               //enter infinite while loop  
 //             printf("\nChecking for button press..");
		if(check_button()){                                        //check if button has been pressed
                        clock_gettime(CLOCK_MONOTONIC, &c1);                    //get clocktime so we can send it through the pipe
                        printf("\nBUTTON PRESS");
			if((write(*((int *)ptr), &c1, sizeof(c1))) < 0){    //write to pipe
                                printf("\nError writing to N_pipe2 in ButtonThread function");
                                exit(-1);
                        }
			else{
				printf("\nPipe operation sucessfull!");
			}
                        clear_button();
                }
             //  read(timer, &num_periods, sizeof(num_periods));
		usleep(75000);
	}

        pthread_exit(0);
}



