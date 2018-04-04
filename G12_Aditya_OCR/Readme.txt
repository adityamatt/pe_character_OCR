Author: Aditya Tiwari
Entry : 2016csb1029
#####################################
OS: Ubuntu 16.04
Requirements: Opencv C++ library
Opencv version: CV_MAJOR_VERSION 3.4.0
User : pkg-config --modversion opencv
in terminal to find the version

How to compile:
g++ compare.cpp -o compare `pkg-config --cflags --libs opencv`
g++ parse.cpp -o parse `pkg-config --cflags --libs opencv`
#####################################
To test compare,
./compare <input image location>
To find all the characters in a given image
./parse <input image location>
Example:
./parse ./Test/mixed.jpg
