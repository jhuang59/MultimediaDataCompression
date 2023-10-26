# MultimediaDataCompression
This repo contain several compression algorithms and examples
# Usage
huffmancoding_global.cpp:
Call encoder(string FilePath,string outputfile) in main function. 

FilePath:File to encode 

outputfile:encoded file

util.py:

Import util.py in jupyter notebook, please find information about the usage of functions in util.py
There are some example usage in ee669HW4Jie.ipynb

class LioydMax():

Lloyd-Max scalar quantizer is in /VectorQuantization/ee669HW3Jie.ipynb. Please find information about the usage in this notebook. There are some example usage inside as well.
# Comparison

Binary Arithmetic Coding with mapping functions

Directly convert one byte to 8 bits
| File |Compressed size|Compression ratios|
|------|---------------|------------------|
|binary.dat|25|0.379|
|text.dat|67|1.01|
|audio.dat|67|1.01|
|image.dat|64|0.97|

Huffman encoding
| File |Compressed size|Compression ratios|
|------|---------------|------------------|
|binary.dat|8|0.121|
|text.dat|37|0.56|
|audio.dat|27|0.409|
|image.dat|39|0.59|

Bit-Plane encoding
| File |Compressed size|Compression ratios|
|------|---------------|------------------|
|binary.dat|9|0.121|
|text.dat|46|0.687|
|audio.dat|63|0.954|
|image.dat|60|0.909|