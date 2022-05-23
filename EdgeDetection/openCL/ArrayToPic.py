# libraries needed to export images, convert from numpy arrays and get input from files
from PIL import Image, ImageOps
from numpy import asarray
import numpy as np

g = open("picInfo.txt", "r") # open file that holds filtered output row, column and pixel count values
LinesInfo = g.readlines() # set LinesInfo to read in file's lines
row = int(LinesInfo[0].strip()) # get the row count
col = int(LinesInfo[1].strip()) # get the column count
pixelCount = int(LinesInfo[2].strip()) # get the pixel count
g.close() # close the file

f = open("Out.txt", "r") # open file that holds filtered output
Lines = f.readlines() # set Lines to read in file's lines
arr = np.zeros((pixelCount,), dtype=np.uint8) # create a numpy array the size of all the pixels found read image originally read in
count = 0 # counter variable

# read in every line from the file and store in the numpy array
for line in Lines:
    arr[count] = int(line.strip())
    count = count + 1

f.close() # close the file
arr3d = np.reshape(arr, (row, col)) # reshape the 1D array to a 2D array

img = Image.fromarray(arr3d) # convert array to an image
img.save('outputOpenCL.png') # save the image
#img.show() # display the image