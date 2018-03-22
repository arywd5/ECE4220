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

#define BUTTON 16
#define PERIOD 75000000
#define PRIORITY 51

typedef struct{					//structure to pass to threads with all the time information 
	struct timespec previous;
	struct timespec next;
	struct timespec hittime;
	int before;
	int after;
	int actual;
} data;

typedef struct{
	int N_pipe2;
	int gpsdata;
	struct timespec t;
} buff;

void *thread0(void *num);			//thread function to print values to the screen after reading from N_pipe1
void process2(int pipe);			//process to complete the second part of this lab 
void *buttonThread(void *ptr);			//thread to check if the button has been pressed each 75 ms
void *interpolate(void *ptr);			//thread to interpolate the data given 


int main(){

	wiringPiSetupGpio();							//set up wiring Pi 
	pinMode(BUTTON, INPUT);							//declare our button as an input 
	pullUpDnControl(BUTTON, PUD_DOWN);


	//declare pipe variables 
	int N_pipe1;
	int dummy;
	buff buffer;

	if((buffer.N_pipe2 = open("N_pipe2", O_RDWR)) < 0){    	       	 	//open pipe
                        printf("N_pipe1 error\n");                              //if the pipe did not open properly exit
                        exit(-1);

	}
	if((dummy = fork()) < 0){						//fork the program and 
		printf("\nError Forking lab4.c");				//check for successful operation
		exit(-1);
	}	

	if(dummy == 0){								//for process 2 use the child 
		 process2(buffer.N_pipe2);
	}

	else{

		pthread_t t0;
		pthread_create(&t0, NULL, (void *)thread0, (void *)&buffer);	//create thread to process 

		if((N_pipe1 = open("/tmp/N_pipe1", O_RDONLY)) < 0){		//open pipe
			printf("N_pipe1 error\n");				//if the pipe did not open properly exit
			exit(-1);
		}

		while(1){							//while loop to read in data from the GPS_device
			if(read(N_pipe1, &(buffer.gpsdata), sizeof(buffer.gpsdata)) < 0){		//read in pipe and store to buffer 
				printf("\nN_pipe1 reading error\n");
				exit(-1);
			}
			clock_gettime(CLOCK_MONOTONIC, &(buffer.t));
		}
		pthread_join(t0, NULL);
	}
	return 0;
}
//thread0 function that prints buffer to the screen to make sure its working properly 
void *thread0(void *num){
	buff buffer = *((buff *)num);
	int dummy;
	data info;
	info.previous = buffer.t;
	info.before = buffer.gpsdata;

	while(1){								//enter infinite while loop 
		if((read(buffer.N_pipe2, &(info.hittime), sizeof(clock))) < 0){	//read in from pipe and check for errors 
			printf("\nError Reading in from N_pipe2 in thread0...");
			exit(-1);
		}
	if((dummy = fork()) < 0){						//fork our process because the button has been pressed 
		printf("\nError forking in thread 0...");			//check for errors 
		exit(-1);
	}	
		if(dummy == 0){							//only our child process will create the new thread while pur main process loops 
			usleep(250000000);					//sleep for 250 ms so buffer will update 
			info.next = buffer.t;
			info.after = buffer.gpsdata;				//set our new values in the next hit	
			pthread_t newThread;					//create a new thread to use the timestamp 
			pthread_create(&newThread, NULL, (void *)interpolate, (void *)&info);
		}
	}

	pthread_exit(0);
}
//second process that will check if the buton has been pressed 
void process2(int pipe){
	
	pthread_t t1; 								//create thread 
	pthread_create(&t1, NULL, (void *)buttonThread, (void *)&pipe);	 	//to check button periodically 
	pthread_join(t1, NULL);							//join thread 

}
//thread function to check wether the button has been pressed 
void *buttonThread(void *ptr){
	int pipe = *((int *)ptr);						//typecast pipe 
	struct timespec c1;							//create a clock to get time with 
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);				//create a timer 
	struct itimerspec itval;

	itval.it_value.tv_sec = 0;						//set the start time of the timer 
	itval.it_value.tv_nsec = 0;
	itval.it_interval.tv_nsec = 0;						//set the period of our timer 
	itval.it_interval.tv_nsec = PERIOD;

	struct sched_param param;						//set up scheduler 
	param.sched_priority = PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	timerfd_settime(timer, 0, &itval, NULL);				//set timer and read first value 
	read(timer, &num_periods, sizeof(num_periods));
		
	while(1){								//enter infinite while loop 
		read(timer, &num_periods, sizeof(num_periods));			//read function 
		if(check_button()){						//check if button has been pressed 
			clock_gettime(CLOCK_MONOTONIC, &c1);			//get clocktime so we can send it through the pipe
			if((write(pipe, &c1, sizeof(c1) != sizeof(c1))) < 0){	//write to pipe
				printf("\nError writing to N_pipe2 in ButtonThread function");
				exit(-1);
			}
		}
		clear_button();	
	}

	pthread_exit(0);
}
//thread function to interpolate the data 
void *interpolate(void *ptr){
	data info = *((data *)ptr);						//typecast our void pointer so we can access the data 
	double slope, intercept;
	
	slope = (double)((info.after - info.before)/(info.previous.tv_nsec - info.next.tv_nsec));
	intercept =(double)((info.next.tv_nsec) - (slope*info.after));
	
	info.actual = (slope*info.hittime.tv_nsec) + intercept;
	printf("\nPrevious GPS_data: %d at time %lf", info.before, info.previous.tv_nsec);
	printf("\nActual GPS_data: %d at time %lf", info.actual, info.hittime.tv_nsec);
	printf("\nAfter GPS_data: %d at time %lf", info.after, info.next.tv_nsec);
}

