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
void *thread1(int find);		//our first thread to search the entire matrix
void *thread2(int row);	  		//second thread to search row by row through the matrix 
void *thread3(int column);		//third thread to seach column by column through the matrix
void *thread4(int index);		//fourth thread to search each element indivdually 

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
//	struct timespec c1, c2;
//	
//	clock_gettime(0, &c1);
		
	//trial I -- one thread to search the entire matrix 
	printf("\nCreating thread variable...");
	pthread_t t1;
	printf("\nCreating thread 1 to search the matrix.....");
	pthread_create(&t1, NULL, (void *)thread1, 0);

	pthread_join(t1, NULL);
	printf("\nThreads joined sucesfully...");

//	clock_gettime(0, &c2);
	printf("\nSearch was sucesfull %d times in 6 seconds", mData.count[0]); //, (c1.tv_sec - c2.tv_sec)*100);

	
	//trial II -- one thread for each row 
	int i;
	int sum = 0;
	pthread_t t2[mData.rows];

	printf("\nbegning trial II...");
	for(i = 0; i < mData.rows; i++){
		printf("\nThread for row %d is being created...", i);
		pthread_create(&(t2[i]), NULL, (void *)thread2, (void *)&i);
		printf("\nsucessful creation.....");
	}
	printf("\nAll threads were sucesfully created....");

	for(i = 0; i < mData.rows; i++){
		pthread_join(t2[i], NULL);
		sum += mData.count[i];
	}
	
	printf("\nTriak II -- Search was sucessful %d times ", sum);

/*	//trial III -- one thread for each column
	int sum3 = 0;
	pthread_t t3;

	for(i = 0; i < mData.columns; i++){
		pthread_create(&t3, NULL, (void *)thread3, (void *)&i);
	}

	for(i = 0; i < mData.columns; i++){
		pthread_join(t3, NULL);
		sum3 += mData.count[i];
	}

	printf("\nTrial III -- Search was sucesful %d times", sum3);

	//trial IV -- one thread for each element in the array 
	int sum4 = 0;
	pthread_t t4;

	for(i = 1; i <= (mData.rows * mData.columns); i++){
		pthread_create(&t4, NULL, (void *)thread4, (void *)&i);
	}

	for(i = 1; i <= (mData.rows * mData.columns); i++){
		pthread_join(t4, NULL);
		sum4 += mData.count[i];
	}

	printf("\ntrial 4 -- Search was sucesfull %d times", sum4);
*/
	return 0;
}
//function to open file and scan in matrix data 
int fileOpen(char *filename){
	FILE *fPtr;
	int i = 0, j = 0;

	fPtr = fopen(filename, "r");
	if(fPtr == NULL){
		printf("\nFile could not be opened please try again.");		
		return 0;
	}
	
		//first scan in the first two elements which hold the number of rows and columns 
		fscanf(fPtr, "%d %d", &(mData.rows), &(mData.columns));
		
		for(i = 0; i < mData.rows; i++){
			for(j = 0; j < mData.columns; j++){
				fscanf(fPtr, "%d", &(mData.matrix[i][j]));
			}
		}
	
	return 1;
}
//function to search through a matric of data and return the amount of times a certain number was found 
void *thread1(int num){
	mData.count[0] = 0; //make sure to 
	int i = 0, j = 0;
	
	for(i = 0; i < mData.rows; i++){
		for(j = 0; j < mData.columns; j++){
			if(mData.find == mData.matrix[i][j]){
				mData.count[0]++;
			}
		}
	}
	pthread_exit(0);
}
//function to search through a specific row of a matrix and count the number of times a certain number was found 
void *thread2(int ro){

	mData.count[ro] = 0;
	int i = 0;
	for(i = 0; i < mData.columns; i++){
		printf("\nelement[%d][%d] = %d == %d", ro, i, mData.matrix[ro][i], mData.find);
		if(mData.matrix[ro][i] == mData.find)
			mData.count[ro]++;
	}
	pthread_exit(0);
}
//function to search each column of an array and count number of time a certain number was found 
void *thread3(int col){

	mData.count[col] = 0;
	int i = 0; 

	for( i = 0; i < mData.rows; i++){
		if(mData.matrix[i][col] == mData.find)
			mData.count[col]++;
	}
	pthread_exit(0);
}
//function to check if a certain element is the same as a given number 
void *thread4(int index){
	int i, j = index;
	mData.count[index] = 0;

	for(i = 0; j >= mData.columns; i++){
		j -= mData.columns;
	}
	
	if(mData.find == mData.matrix[i][j]){
		mData.count[index]++;
	}
	pthread_exit(0);
}
