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

int main(){


	wiringPiSetupGpio();                                                    //set up wiring Pi
	pinMode(BUTTON, INPUT);                                                 //declare our button as an input
	pullUpDnControl(BUTTON, PUD_DOWN);
	int N_pipe2;

	struct timespec c1;                                                     //create a clock to get time with
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);                         //create a timer
	struct itimerspec itval;

	itval.it_value.tv_sec = 0;                                              //set the start time of the timer
	itval.it_value.tv_nsec = 1000;
	itval.it_interval.tv_nsec = 0;                                          //set the period of our timer
	itval.it_interval.tv_nsec = PERIOD;

	struct sched_param param;                                               //set up scheduler
	param.sched_priority = PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	if((N_pipe2 = open("N_pipe2", O_RDWR)) < 0){
                                printf("\nPipe opening error in process 2 ");
                                exit(-1);
                        }



	timerfd_settime(timer, 0, &itval, NULL);                                //set timer and read first value
	read(timer, &num_periods, sizeof(num_periods));
	printf("\n Entering While loop...");
	while(1){                                                               //enter infinite while loop
		
		if(check_button()){                                        //check if button has been pressed
			clock_gettime(CLOCK_MONOTONIC, &c1);                    //get clocktime so we can send it through the pipe
			printf("\nBUTTON PRESS");
			if((write(N_pipe2, &c1, sizeof(c1))) < 0){    //write to pipe
				printf("\nError writing to N_pipe2 in ButtonThread function");
				exit(-1);
			}
			else{
				printf("\nPipe operation sucessfull!");
			}
			clear_button();
		}
		  read(timer, &num_periods, sizeof(num_periods));
//		usleep(75000);
	}




	return 0;
}
