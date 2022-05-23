# libraries needed to export images, convert from numpy arrays and get input from files
from PIL import Image, ImageOps
from numpy import asarray
import numpy as np

img = Image.open('outputGolden.png') # read in an image
img = ImageOps.grayscale(img) # convert image to grayscale
arr = asarray(img) # convert image to 2D array
write = arr.flatten() # convert 2D array to 1D array

img2 = Image.open('outputOpenCL.png') # read in an image
img2 = ImageOps.grayscale(img2) # convert image to grayscale
arr2 = asarray(img2) # convert image to 2D array
write2 = arr2.flatten() # convert 2D array to 1D array

count = 0
for i in range (write2.size):
    if (write[i] > write2[i] + 1 or write[i] < write2[i] + 1):
        count = count + 1
count =  float(count)/(write2.size)
print("Accuracy metric: " + str(count))
print("Does the golden measure image match the OpenCL image: " + str(np.array_equal(write, write2)))
#print("Does the golden measure image match the OpenCL image: " + str(np.array_equal(write, write2)))

g = open("OutMedianGoldenSpeed.txt", "r") # open file that holds golden speed
LinesInfo = g.readlines() # set LinesInfo to read in file's lines
speedGolden = float(LinesInfo[0].strip()) # get the speed golden
g.close() # close the file

f = open("OutMedianOpenCLSpeed.txt", "r") # open file that holds OpenCL speed
LinesInfo2 = f.readlines() # set LinesInfo to read in file's lines
speedOpenCl = float(LinesInfo2[0].strip()) # get the speed OpenCL
overheadOpenCl = float(LinesInfo2[1].strip()) # get the speed OpenCL overhead
f.close() # close the file

calc = speedGolden/speedOpenCl
print("Speed up of OpenCL to golden of median filter (without overhead): " + str(calc))
calc = speedGolden/(speedOpenCl+overheadOpenCl)
print("Speed up of OpenCL to golden of median filter (with overhead): " + str(calc))