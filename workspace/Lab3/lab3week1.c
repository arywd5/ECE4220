//Allison Yaeger 
//14244528

//libraries 
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
//header files 
#include "ece4220lab3.h"

#define RL 2			//pin number for the red led 
#define GL 4			//pin number for the green led 
#define YL 3			//pin number for the yellow led 
#define BUTTON 16		//pin number for the psuh button 
#define GLP 51			//prioriy for the green light 
#define RLP 52			//priority for the red light 
#define YLP 51			//priority for the yellow light 
#define PERIOD 2

void *runLights(void *ptr);		//thread function to control the lights  

typedef struct arguments{
	int waitnsec;
	int pinnum;
	int priority;
	long nperiod;
	
}args;


int main(){

	//setup function 
	wiringPiSetupGpio();

	//define pin modes for the LEDs and buttons we will be using during this lab 
	pinMode(RL, OUTPUT);
	pinMode(GL, OUTPUT);
	pinMode(YL, OUTPUT);
	pinMode(BUTTON, INPUT);
	pinMode(20, INPUT);

	digitalWrite(RL, 0);
	digitalWrite(YL, 0);
	digitalWrite(GL, 0);

	pthread_t p1;
	pthread_create(&p1, NULL, (void*)runLights, NULL);

	pthread_join(p1, NULL);

	return 0;
}
//thread function to control the traffic lights 
void *runLights(void *ptr){

	 while(!digitalRead(20)){			//while loop to control the lights 
                if(check_button()){			//first check if the button has been pressed 
                        digitalWrite(RL, 1);		//then turn the light on 
                        sleep(PERIOD);
                        digitalWrite(RL, 0);
                        clear_button();
                }

                digitalWrite(YL, 1);			//turn on yellow loght 
                sleep(PERIOD);
                digitalWrite(YL, 0);			

                 if(check_button()){			//check again if the button has been pressed 
                        digitalWrite(RL, 1);		//if so turn on red light and clear button
                        sleep(PERIOD);
                        digitalWrite(RL, 0);
                        clear_button();
                }

                digitalWrite(GL, 1);			//turn on green light 
                sleep(PERIOD);
                digitalWrite(GL, 0);	
	}

	pthread_exit(0);
}

