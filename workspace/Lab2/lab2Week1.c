//Allison Yaeger 
//14244528
//ECE 42240 Lab2 Week1

#include <stdio.h>
#include <sys/time.h>
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



data *fileOpen(char *filename); //this function will open the file and inport the data into an array 
void *thread1(data mData);		//our first thread to search the entire matrix
void *thread2(data mData);		//second thread to search row by row through the matrix 
void *thread3(data mData);		//third thread to seach column by column through the matrix

int main(int argc, char *argv[]){
	
	if(argc != 3){	//first check for the correct amount of input arguments. 
		printf("\nIncorrect Usage\n ./a.out <number to search> <file to search>");
		return -1;
	}
	
	data *mData = fileOpen(argv[2]);
	
	//trial I -- one thread to search the entire matrix 
	pthread_t t1;
	pthread_create(t1, NULL, thread1, (void *)mData);
	
	pthread_join(t1, NULL);
	printf("\nSearch was sucesfull %d times", mData->count[0]);
	
	
	
	
	
	return 0;
}

data *fileOpen(char *filename){
	FILE *fPtr;
	data mData;
	int i = 0, j = 0;
	
	printf("\nScanning in data....");
	
	fPtr = fopen(filename, "r");
	if(fPtr == NULL){
		printf("\nFile could not be opened please try again.");
		return NULL;		
	}
	
	//first scan in the first two elements which hold the number of rows and columns 
	fscanf(fPtr, "%d %d", &(mData.rows), &(mData.columns));
	
	for(i = 0; i < mData.rows; i++){
		for(j = 0; j < mData.columns; j++){
			fscanf(fPtr, "%d", &(mData.matrix[i][j]));
		}
	}
			
	
	return &mData;
	
}

void *thread1(data mData){
	mData.count[0] = 0; //make sure to 
	int i = 0, j = 0;
	for(i = 0; i < mData.rows; i++){
		for(j = 0; j < mData.columns; j++){
			if(mData.find == mData.matrix[i][j])
				mData.count[0]++;
		}
	}
	pthread_exit(1);
}

void *thread2(data mData){
	
	int i = 0;
	for(i = 0; i < mData.Columns; i++){
		if(mData.matrix[][i] == mData.find)
			count[]++;
	}
	
}