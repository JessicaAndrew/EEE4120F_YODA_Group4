# EEE4120F_YODA_Group4

Group Members:
Jessica Andrew - ANDJES006
Skye Zive - ZVXSKY001
Nicholas Rogers - RGRNIC007

To run the pre-processing the image of choice need to be put in the same folder and the name within PicToArray.py (line 6) needs to be changed to match. it can then be run. Then the two output text files need to be put in the same directory as the golden filters and the cpp files for both the median filter and the edge detection methods. All these code files can then be run. The text files created (out.txt) can then be converted back into an image by running the version of ArrayToPic.py in the current folder. Note that additionally if you put the output images and text files into the same folder as the compare code you can then see the output for the various codes.


Command line statements to run the various codes:
python PicToArray.py
python ArrayToPic.py

g++ edgeDetectionSeq.cpp -o edgeDetectionSeq.out -lOpenCL
./edgeDetectionSeq.out

g++ EdgeDetection.cpp -o EdgeDetection.out -lOpenCL
./EdgeDetection.out

g++ medianFilterGoldenStandard.cpp -o medianFilterGoldenStandard.out -lOpenCL
./medianFilterGoldenStandard.out

g++ medianFilter.cpp -o medianFilter.out -lOpenCL
./medianFilter.out

python compare.py


Example run order for the OpenCL median filter:
python PicToArray.py
g++ medianFilter.cpp -o medianFilter.out -lOpenCL
./medianFilter.out
python ArrayToPic.py
python compare.py
