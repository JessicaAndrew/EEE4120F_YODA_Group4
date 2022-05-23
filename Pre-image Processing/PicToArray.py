# libraries needed to import images, convert to numpy arrays and output to files
from PIL import Image, ImageOps
from numpy import asarray
import numpy

img = Image.open('YodaXray.png') # read in an image
img = ImageOps.grayscale(img) # convert image to grayscale
arr = asarray(img) # convert image to 2D array
write = arr.flatten() # convert 2D array to 1D array

f = open("In.txt", "w") # open file for printing array values to

# send every element in array to text file using a for loop and close the file
for i in range(write.size):
    f.write(str(write[i]) + "\n")
f.close() # close the file

g = open("picInfo.txt", "w") # open file for printing array values to
g.write(str(arr.shape[0]) + "\n") # send row count to text file
g.write(str(arr.shape[1]) + "\n") # send column count to text file
g.write(str(arr.size) + "\n") # send total pixel count to text file
g.close() # close the file

img2 = Image.fromarray(arr) # convert array to an image
img2.save('inputGreyScale.png') # save the image
#img.show() # display the image