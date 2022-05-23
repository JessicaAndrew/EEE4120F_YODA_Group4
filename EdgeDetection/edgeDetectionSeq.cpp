//Author: Skye Zive For the EEE4120F YODA project at UCT

#include<stdio.h>
#include<iostream>
#include<math.h>
using namespace std;

//matrices used to compute the respective X and Y gradients of the input image
int xKern[9] = {-1,0,1,-2,0,2,-1,0,1};
int yKern[9] = {-1,-2,-1,0,0,0,1,2,1};

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

// function to read in pixel values from the input file an store in an array (using a passed in pointer reference)
void create1DImageMatrix(int size, int* matrixValues){
    FILE *in_file = fopen("In.txt", "r"); //read in file
    int temp;
    int temp2 = 0; // value to allow for debugging where the size of the output pixel array needs to be limited
    int i = 0; // counter value

    // code to scan in the file line by line
    while (fscanf(in_file, "%d", & temp ) == 1 && temp2<=size){
    matrixValues[i] = temp;
    i = i + 1;
    temp2 = temp2 +1;
    }

}

int main(){
    clock_t startProcess, endProcess, startPrint1, endPrint1, startPrint2, endPrint2;  //Timers
	startProcess = clock(); //start the clock for timing the golden standard running
	
    int picInfo[3];
    getPicInfo(picInfo);
    int count = picInfo[2]; //total number of pixels in the image 
    int width = picInfo[1]; //the number of columns (pixels) in each row
    int depth = picInfo[0]; // the number of rows of pixels
    int matrixValues[count];
    create1DImageMatrix(count, matrixValues);

    int outputArr[count];

    int row = 0;
    int col = 0;
    int xGrad = 0;
    int yGrad = 0;
    float sobelGrad = 0.0;

    for (int row = 0; row<depth; row++){ //iterates through each row in the image
        for (int col = 0; col<width; col++){ //iterates through each pixel in the row

            // if pixel is on edge of the image
            if(row == 0 || row == depth-1 || col == 0 || col == width-1){

                outputArr[row*width + col] = 0; //exclude because neg.

            }
        
            else{
            // if the pixel is in middle of the image
            //calculate the gradient of each pixel in the image in the x and y directions
                xGrad = matrixValues[(row*width + col)-1-width]*xKern[0] + matrixValues[(row*width + col)-width]*xKern[1] + matrixValues[(row*width + col)+1-width]*xKern[2] + matrixValues[(row*width + col)-1]*xKern[3] + matrixValues[(row*width + col)]*xKern[4] + matrixValues[(row*width + col)+1]*xKern[5] + matrixValues[(row*width + col)-1+width]*xKern[6] + matrixValues[(row*width + col)+width]*xKern[7] + matrixValues[(row*width + col)+1+width]*xKern[8];
                yGrad = matrixValues[(row*width + col)-1-width]*yKern[0] + matrixValues[(row*width + col)-width]*yKern[1] + matrixValues[(row*width + col)+1-width]*yKern[2] + matrixValues[(row*width + col)-1]*yKern[3] + matrixValues[(row*width + col)]*yKern[4] + matrixValues[(row*width + col)+1]*yKern[5] + matrixValues[(row*width + col)-1+width]*yKern[6] + matrixValues[(row*width + col)+width]*yKern[7] + matrixValues[(row*width + col)+1+width]*yKern[8];                
                sobelGrad = sqrt(pow(xGrad,2)+pow(yGrad,2)); //combination of the x and y gradients
                outputArr[(row*width + col)] = sobelGrad; // the output array contains the gradient of each pixel
            }
            
            
            
        }
    }

    endProcess = clock(); //end the clock for timing the golden standard running
	startPrint1 = clock(); //start the first clock for timing the golden standard print
	

    // //printing out output pixel matrix
	// for(int n = 0; n<count; n++){
	// 	printf("%i	" ,outputArr[n]);

	// 	if(n%(width) == (width-1)){
	// 			printf("\n");
	// 	}
	// }

    //sending output pixel values to a text file
	FILE *filePrint = fopen("Out.txt","w");
	for(int n = 0; n<(count); n++){
		fprintf(filePrint,"%d\n", outputArr[n]);
   		
	}
	fclose(filePrint);

    endPrint1 = clock(); //end the first clock for timing the golden standard print

	printf ("Run Time of edge dectection (golden measure): %0.8f sec \n", ((float) endProcess - startProcess)/CLOCKS_PER_SEC); //print the timing of the golden standard
	printf ("Save to file Run Time of edge dectection (golden measure): %0.8f sec \n", ((float) endPrint1 - startPrint1)/CLOCKS_PER_SEC); //print the timing of the golden standard
	FILE *filePrintInfo = fopen("OutEdgeGoldenSpeed.txt","w");
	fprintf(filePrintInfo,"%0.8f\n", (((float) endProcess - startProcess)/CLOCKS_PER_SEC));
	fclose(filePrintInfo);

    return 0;


}

