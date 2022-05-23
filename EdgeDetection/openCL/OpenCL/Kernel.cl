//set arguments for the kernel
__kernel void edgeDetection(__global int* imgMatrix, __global int* output, __global int* rowSize, __global int* colSize, __global int* countPixels, __global int* xKern, __global int* yKern){
	//note that each work group calculates 1 row for the output matrix
	//note that each work item calculates each value (column) in 1 row in the output matrix

	//Work item and work groups numbers
	int workItemNum = get_global_id(0); //Work item ID
	int workGroupNum = get_group_id(0); //Work group ID
	int localGroupID = get_local_id(0); //Work items ID within each work group

	int xGrad = 0;
    int yGrad = 0;
    int sobelGrad = 0; //change to int
	
	//Memory buffers
	uint global_addr = workItemNum;
	int row = *rowSize; //To get the number of rows of the input matrix
	int col = *colSize; //To get the number of columns of the input matrix
	int pixelCount = *countPixels; //To get the number of pixels in the input matrix
	int c_row = workItemNum/col;
	int c_col = workItemNum%col;

	if(c_row == 0 || c_row == row-1 || c_col == 0 || c_col == col-1){
		output[workItemNum] = 0; 
	}else{
		// if the pixel is in middle of the image
        //calculate the gradient of each pixel in the image in the x and y directions
        xGrad = imgMatrix[(c_row*col + c_col)-1- col]*xKern[0] + imgMatrix[(c_row*col + c_col)-col]*xKern[1] + imgMatrix[(c_row*col + c_col)+1-col]*xKern[2] + imgMatrix[(c_row*col + c_col)-1]*xKern[3] + imgMatrix[(c_row*col + c_col)]*xKern[4] + imgMatrix[(c_row*col + c_col)+1]*xKern[5] + imgMatrix[(c_row*col + c_col)-1+col]*xKern[6] + imgMatrix[(c_row*col + c_col)+col]*xKern[7] + imgMatrix[(c_row*col + c_col)+1+col]*xKern[8];
        yGrad = imgMatrix[(c_row*col + c_col)-1- col]*yKern[0] + imgMatrix[(c_row*col + c_col)-col]*yKern[1] + imgMatrix[(c_row*col + c_col)+1-col]*yKern[2] + imgMatrix[(c_row*col + c_col)-1]*yKern[3] + imgMatrix[(c_row*col + c_col)]*yKern[4] + imgMatrix[(c_row*col + c_col)+1]*yKern[5] + imgMatrix[(c_row*col + c_col)-1+col]*yKern[6] + imgMatrix[(c_row*col + c_col)+col]*yKern[7] + imgMatrix[(c_row*col + c_col)+1+col]*yKern[8];
        sobelGrad = (int)(sqrt((float)(xGrad*xGrad+yGrad*yGrad))); //combination of the x and y gradients 
        output[workItemNum] = sobelGrad; // the output array contains the gradient of each pixel
    }
}