/*
 ============================================================================
 Name        : testProject.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define RED_LED 2
#define BLUE_LED 5
#define SPEAKER 6


int main(void) {

	wiringPiSetupGpio();

	int choice;
	pinMode(RED_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(SPEAKER, OUTPUT);
	pinMode(16, INPUT);
	pinMode(17, INPUT);
	pinMode(18, INPUT);
	pinMode(19, INPUT);
	pinMode(10, INPUT);	

	printf("\nEnter 1 for part 1 and 2 for part 2: ");
	scanf("%d", &choice);

	if(choice == 1){
		while(1){
			digitalWrite(RED_LED, 1);
			sleep(1);
			digitalWrite(RED_LED, 0);
			digitalWrite(BLUE_LED, 1);
			sleep(1);
			digitalWrite(BLUE_LED, 0);
		}
	}

	if(choice == 2){
		int button = -1;
		printf("\nPlease enter a number 1-5 for a button: ");
		while(button < 1 || button > 5){
			scanf("%d", &button);
		}
		
		button += 15;		//add 15 to the button to make it correspond to the pin numbers 		
	
		pullUpDnControl(button, PUD_DOWN);
		while(!digitalRead(button)){}	
			while(1){
				digitalWrite(SPEAKER, 1);
				usleep(700);
				digitalWrite(SPEAKER, 0);
				usleep(700);			
			}
	}
	return 0;
}
