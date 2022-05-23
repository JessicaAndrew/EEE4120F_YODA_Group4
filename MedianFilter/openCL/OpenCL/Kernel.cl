//set arguments for the kernel
__kernel void medianFilter(__global int* imgMatrix, __global int* output, __global int* rowSize, __global int* colSize, __global int* countPixels, __global int* filterSize){
	//note that each work group calculates 1 row for the output matrix
	//note that each work item calculates each value (column) in 1 row in the output matrix

	//Work item and work groups numbers
	int workItemNum = get_global_id(0); //Work item ID
	int workGroupNum = get_group_id(0); //Work group ID
	int localGroupID = get_local_id(0); //Work items ID within each work group
	
	//Memory buffers
	uint global_addr = workItemNum;
	int row = *rowSize; //To get the number of rows of the input matrix
	int col = *colSize; //To get the number of columns of the input matrix
	int pixelCount = *countPixels; //To get the number of pixels in the input matrix
	int sizeFilter = *filterSize; //To get the filter size of the median filter

	int counter = 0; // counting variable
	int n = (int)(sqrt((double)(sizeFilter+1))); 
	int move = (int)(sqrt((double)(sizeFilter+1))/2); // total number of moves needed (number of layers of closet neighbours)
	int sizeMedianArray = 400; // size used for temp array for medain filtering, note OpenCL doesn't allow variable array instantiations in its kernal so this has to be hardcoded
	int output1[400]; // temp array for output values to be median filtered for each pixel window
	int tempSwap = 0; // temp value used in the bubble sort algorithm used to find the median
	output[workItemNum] = 0; //set output value to zero in case any junk is still there
		
	// reset all used temp values to 0 for next iteration
	counter = 0;
	for (int z = 0; z < sizeMedianArray; z++){
		output1[z] = 0;
	}

	for(int z=0; z<n; z++){ // go through filter block's rows
		for(int x=0; x<n; x++){ // got through filter block's columns
			// if statement that checks that the filter is kept within the bounds of the image: first two do row bounds (i.e. to stop values from accessing data in previous row) and last two do image bounds (i.e. first and last pixel)
			if((workItemNum -col*move + z*col + 0 +x -move) >= (workGroupNum*col + z*col + 0 - move*col) && (workItemNum -col*move + z*col + 0 +x -move) <= (workGroupNum*col - 1 + col + z*col + 0 - move*col) && (workItemNum -col*move + z*col + 0 +x -move) < (col*row) && (workItemNum -col*move + z*col + 0 +x -move)>=0){
				output1[counter] = imgMatrix[workItemNum -col*move + z*col + 0 +x -move]; // send value in filter to temp output array to be sorted
				counter = counter + 1; // count the number of elements in the temp output array
			}
		}
	}
	
	// for statement to run through values in temp array for sorting using bubble sort algorithm to sort in ascending order
	for (int a = 0; a < (sizeFilter + 1); a++){
		for (int b = 0; b < (sizeFilter + 1 - a); b++){
			if (output1[b] < output1[b + 1]){ // set the array to be in descending order
				tempSwap = output1[b];
				output1[b] = output1[b + 1];
				output1[b + 1] = tempSwap;
			}
		}
	}
	output[workItemNum] = output1[(int)(counter/2)]; // set value in output array to median of filter block (using bubble sorted temp output array)
}