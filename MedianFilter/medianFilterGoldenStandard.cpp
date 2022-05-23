// include needed libraries
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
using namespace std;

// function to read in picture information (row, column, pixel count) from the input file and store (using a passed in pointer reference) in an array
void getPicInfo(int* picInfo){
	FILE *inFile  = fopen("picInfo.txt", "r"); //read in file
	int temp;
	int i = 0; // counter value

	// code to scan in the file line by line
	while (fscanf(inFile, "%d", & temp ) == 1){ 
		picInfo[i] = temp;
		i = i + 1;
	} 
}

// function to read in pixel values from the input file and store (using a passed in pointer reference) in an array
void create1DImageMatrix(int size, int* matrixValues){
	FILE *inFile  = fopen("In.txt", "r"); //read in file
	int temp;
	int temp2 = 0; // value to allow for debugging where the size of the output pixel array needs to be limited
	int i = 0; // counter value

	// code to scan in the file line by line
	while (fscanf(inFile, "%d", & temp ) == 1 && temp2<=size){ 
		matrixValues[i] = temp;
		i = i + 1;
		temp2 = temp2 +1;
	} 
}

// main method that performs the median filtering
int main(void){
	clock_t startProcess, endProcess, startPrint1, endPrint1, startPrint2, endPrint2;  //Timers
	startProcess = clock(); //start the clock for timing the golden standard running
	
	int picInfo[3]; // array to store row, column and pixel counts
	getPicInfo(picInfo); // get picture information

	int countPixels = picInfo[2];// total numbr of pixels in the image
	int matrixValues[countPixels]; // array to store pixel values
	create1DImageMatrix(countPixels,matrixValues); // get picture's pixels

	int rowSize = picInfo[0];// total numbr of rows in the image
	int colSize = picInfo[1];// total numbr of columns in the image
	int filterSize = 80; // immediate neighbour number
	int output[countPixels]; // output array of filtered values
	float divide1 = 0.0; // counting variable
	int n = (int)(sqrt(double (filterSize+1))); 
	int move = (int)(sqrt(double (filterSize+1))/2); // total number of moves needed (number of layers of closet neighbours)
	int temp5 = 0; // check variable used in moderating the filter size
	int temp6 = 0; // check variable used in moderating the filter size
	int output1[filterSize+1]; // temp array for output values to be median filtered for each pixel window
	int tempSwap = 0; // temp value used in the bubble sort algorithm used to find the median
	
	// set all values in the temp output array to 0
	for (int z = 0; z < (filterSize+1); z++){
		output1[z] = 0;
	}

	for (int row = 0; row<rowSize; row++){ //iterates through each row of input matrix
		for (int col = 0; col<colSize; col++){ //iterates through each column of input matrix
			output[row*colSize + col]=0; //sets the output of each index in the matrix to 0
			
			// checks used in moderating the filter size for edges of the image
			temp5 = row-move;
			if (temp5 >= 0){
				temp5 = 0;
			}
			temp6 = row-1;
			if (temp6 <= 0){
				temp6 = 0;
			}
			
			for(int z=0; z<(n+temp5); z++){ // go through filter block's rows
				for(int x=0; x<n; x++){ // got through filter block's columns
					// if statement that checks that the filter is kept within the bounds of the image
					if((z*colSize + x - move + col + colSize*temp6) >= (colSize*z + colSize*temp6) && (z*colSize + x - move + col + colSize*temp6) <= (colSize*z + colSize - 1 + colSize*temp6) && (z*colSize + x - move + col + colSize*temp6) < (colSize*rowSize)){
						output1[int(divide1)] = matrixValues[z*colSize + x - move + col + colSize*temp6]; // send value in filter to temp output array to be sorted
						divide1 = divide1 + 1; // count the number of elements in the temp output array
						//printf("%i %i || %i %i %i\n", colSize*z, colSize*temp6, z , temp6, colSize);
					}
				}
			}
			
			// for statement to run through values in temp array for sorting using bubble sort algorithm to sort in ascending order
			for (int a = 0; a < (filterSize + 1); a++){
				for (int b = 0; b < (filterSize - a + 1); b++){
					if (output1[b] < output1[b + 1]){
						tempSwap = output1[b];
                   		output1[b] = output1[b + 1];
                   		output1[b + 1] = tempSwap;
					}
				}
			}
			//filterSize + 1 - int(round(divide1/2))
			output[row*colSize + col] = output1[(int)(divide1/2)]; // set value in output array to median of filter block (using bubble sorted temp output array)
			
			// reset all used temp values to 0 for next iteration
			divide1 = 0;
			for (int z = 0; z < (filterSize+1); z++){
				output1[z] = 0;
			}

		}
	}
	
	endProcess = clock(); //end the clock for timing the golden standard running
	startPrint1 = clock(); //start the first clock for timing the golden standard print
	

	//printing out input pixel matrix
	/*for(int i = 0; i<countPixels; i++){
		printf("%i	" ,matrixValues[i]);

		if(i%(colSize) == (colSize-1)){
				printf("\n");
		}
	}
	printf("\n\n\n");
	//printing out output pixel matrix
	for(int n = 0; n<countPixels; n++){
		printf("%i	" ,output[n]);

		if(n%(colSize) == (colSize-1)){
				printf("\n");
		}
	}*/
	
	//sending output pixel values to a text file
	FILE *filePrint = fopen("Out.txt","w");
	for(int n = 0; n<(countPixels); n++){
		// if statement used to not put a new line for the last line
   		if (n != (countPixels - 1)){
			fprintf(filePrint,"%d\n", output[n]);
		}else{
			fprintf(filePrint,"%d", output[n]);
		}
	}
	fclose(filePrint);

	endPrint1 = clock(); //end the first clock for timing the golden standard print

	printf ("Run Time of median filter (golden measure): %0.8f sec \n", (((float) endProcess - startProcess)/CLOCKS_PER_SEC)); //print the timing of the golden standard
	printf ("Save to file Run Time of median filter (golden measure): %0.8f sec \n", ((float) endPrint1 - startPrint1)/CLOCKS_PER_SEC); //print the timing of the golden standard
	FILE *filePrintInfo = fopen("OutMedianGoldenSpeed.txt","w");
	fprintf(filePrintInfo,"%0.8f\n", (((float) endProcess - startProcess)/CLOCKS_PER_SEC));
	fclose(filePrintInfo);

	return 0;
}