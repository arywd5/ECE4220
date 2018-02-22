//Allison Yaeger 
//14244528
//ECE 42240 Lab2 Week1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define ROWS 150
#define COLUMNS 150
#define THREADS 500



typedef struct {					//structure to hold the data from the file
	int matrix[ROWS][COLUMNS];
	int count[THREADS];			//make an array so we can use a seperate count for each thread process 
	int rows;
	int columns; 
	int find;

} data;

data mData;

int fileOpen(char *filename);  //this function will open the file and inport the data into an array 
void *thread1(void *ptr);		//our first thread to search the entire matrix
void *thread2(void *ptr);	  		//second thread to search row by row through the matrix 
void *thread3(void *ptr);		//third thread to seach column by column through the matrix
void *thread4(void *ptr);		//fourth thread to search each element indivdually 

int main(int argc, char *argv[]){

	if(argc != 3){	//first check for the correct amount of input arguments. 
		printf("\nIncorrect Usage\n ./a.out <number to search> <file to search>");
		return -1;
	}

	if(!atoi(argv[1])){	//check that the number given can be converted to an integer 
		printf("\nIncoreect Usage\n Please enter a valid number to search\n./a.out <number to search> <file to search>");
		return -1;
	}

	if(fileOpen(argv[2]) == 0){	//if this function does not work properaly it will return a 0 and we should exit the program
		printf("\nCorrect Usage:\n./a.out <number to search> <file to search>");
		return -1;
	}

	mData.find = atoi(argv[1]);	//set our number to find in our array 

	//declare time variables 
	struct timespec c1, c2;
	int nums[THREADS];
	int i  = 0;
	for(i = 0; i < THREADS; i++){
		nums[i] = i;
	}

		clock_gettime(CLOCK_MONOTONIC, &c1);

		//trial I -- one thread to search the entire matrix ;
		pthread_t t1;
		pthread_create(&t1, NULL, (void *)thread1, 0);

		pthread_join(t1, NULL);

		clock_gettime(CLOCK_MONOTONIC, &c2);
		printf("/nTrial I -- Search was sucesfull %d times in %ld ms", mData.count[0], (c2.tv_nsec - c1.tv_nsec)/100);
		

		//trial II -- one thread for each row 
		int sum = 0;
		pthread_t t2[mData.rows];									//create array of threads to use in for loop

		clock_gettime(CLOCK_MONOTONIC, &c1);						//get current time before the search 
		for(i = 0; i < mData.rows; i++){							//for loop to create a thread for reach row 
			pthread_create(&(t2[i]), NULL, (void *)thread2, (void *)&nums[i]);
		}

		for(i = 0; i < mData.rows; i++){							//for loop to join thread for each row 
			pthread_join(t2[i], NULL);
			sum += mData.count[i];									//sum our counts in this for loop 
		}
		clock_gettime(CLOCK_MONOTONIC, &c2);						//get ending time for this trial 
		//print results to user 
		printf("\nTriak II -- Search was sucessful %d times in %ld ms", sum, (c2.tv_nsec - c1.tv_nsec)/100);
		


		//trial III -- one thread for each column
		int sum3 = 0;
		pthread_t t3;

		clock_gettime(CLOCK_MONOTONIC, &c1);						//get clock time before the search 
		for(i = 0; i < mData.columns; i++){							//for loop to create a thread for each column
			pthread_create(&t3, NULL, (void *)thread3, (void *)&nums[i]);
		}

		for(i = 0; i < mData.columns; i++){							//for loop to join threads for each columns 
			pthread_join(t3, NULL);		
			sum3 += mData.count[i];									//also sum our counters
		}
		clock_gettime(CLOCK_MONOTONIC, &c2);						//get clock time after search is complete 
		//prut results to the user 
		printf("\nTrial III -- Search was sucesful %d times in %ld ms", sum3, (c2.tv_nsec - c1.tv_nsec)/100);
		
	

		//trial IV -- one thread for each element in the array 
		int sum4 = 0;
		pthread_t t4;

		clock_gettime(CLOCK_MONOTONIC, &c1);						//get clocktime before the search has started 
		for(i = 1; i <= (mData.rows * mData.columns); i++){			//for loop to create thread for each element of the array 
			pthread_create(&t4, NULL, (void *)thread4, (void *)&nums[i]);
		}

		for(i = 1; i <= (mData.rows * mData.columns); i++){			//for loop to join threads for each element 
			pthread_join(t4, NULL);
			sum4 += mData.count[i];									//sum counters 
		}
		clock_gettime(CLOCK_MONOTONIC, &c2);						//get clock time after search is finished 
		//print results to user 
		printf("\ntrial 4 -- Search was sucesfull %d times in %ld ms", sum4, (c2.tv_nsec - c1.tv_nsec)/100);
		
	return 0;														//exit function 
}

//function to open file and scan in matrix data 
int fileOpen(char *filename){
	FILE *fPtr;
	int i = 0, j = 0;

	fPtr = fopen(filename, "r");									//open file for reading 
	if(fPtr == NULL){												//check that file opened properly 
		printf("\nFile could not be opened please try again.");		
		return 0;
	}

	//first scan in the first two elements which hold the number of rows and columns 
	fscanf(fPtr, "%d %d", &(mData.rows), &(mData.columns));			

	for(i = 0; i < mData.rows; i++){								//for loop to gp through rows 
		for(j = 0; j < mData.columns; j++){							//for loop to go through each element in the column 
			fscanf(fPtr, "%d", &(mData.matrix[i][j]));				//scan in data and save to our fixed matrix 
		}
	}

	fclose(fPtr);													//close file to avoid memory leak
	return 1;
}
//function to search through a matric of data and return the amount of times a certain number was found 
void *thread1(void *ptr){
	mData.count[0] = 0; 											//make sure to initialize count to 0 
	int i = 0, j = 0;

	for(i = 0; i < mData.rows; i++){								//use nested for loops to parse array 
		for(j = 0; j < mData.columns; j++){
			if(mData.find == mData.matrix[i][j]){					//if the variable we are finding is the same 
				mData.count[0]++;										//incremeent the counter 
			}
		}
	}
	pthread_exit(0);
}
//function to search through a specific row of a matrix and count the number of times a certain number was found 
void *thread2(void *ptr){

	int ro = (*(int *)ptr);											//type case pointer so we can access it 
	mData.count[ro] = 0;											//intiazlize our count to zero 
	int i = 0;	
	for(i = 0; i < mData.columns; i++){								//for loop to parse through all the elements in a row 
		if(mData.matrix[ro][i] == mData.find)
			mData.count[ro]++;										//increment counter if the element matched our find variable 
	}
	pthread_exit(0);
}
//function to search each column of an array and count number of time a certain number was found 
void *thread3(void *ptr){

	int col = *((int *)ptr);										//type case pointer so we can use this variable 
	mData.count[col] = 0;											//initialize our counter to zero 
	int i = 0; 

	for( i = 0; i < mData.rows; i++){								//for loop to go through each row of the specified column 
		if(mData.matrix[i][col] == mData.find)
			mData.count[col]++;										//if the elements equal then increment our counter 
	}
	pthread_exit(0);
}
//function to check if a certain element is the same as a given number 
void *thread4(void *ptr){
	int index = *((int *)ptr);										//type cast the pointer to access the element number 
	int i, j = index;
	mData.count[index] = 0;

	for(i = 0; j >= mData.columns; i++){							//for loop to find the row and column numbers
		j -= mData.columns;											// subtract the number of rows from the index as long as its greater than the
	}																//number of columns, where this for loop breaks i & j will respectivley be [i][j] row and column

	if(mData.find == mData.matrix[i][j]){							//checkt if our element is equalt to the to find variable 
		mData.count[index]++;										//increment counter 
	}
	pthread_exit(0);
}
