# MultimediaDataCompression
This repo contain several compression algorithms and examples
# Usage
huffmancoding_global.cpp:
Call encoder(string FilePath,string outputfile) in main function. 

FilePath:File to encode 

outputfile:encoded file
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