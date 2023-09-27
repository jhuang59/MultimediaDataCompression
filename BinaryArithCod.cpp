//Copy right:Jie Huang
//https://github.com/jhuang59/MultimediaDataCompression
#include "QMCODER.CPP"
#include "huffmancoding_global.cpp"
#include "QMCODER.H"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

//===============================//
//Mapping function
//c: char represented in ASCII
std::bitset<8> byte2binaryDriect(int c){
    if (c >= 0 && c <= 255) {
        std::bitset<8> binary(c);
        return binary;
    }
}
//huffman coding mapping function
std::unordered_map<int,std::vector<int> > huffmanMapping(string FilePath){
    
    //open the file
    ifstream inputFile(FilePath, ios::binary);
    //original data
    vector<unsigned char> data((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    //count the frequency of chars
    map<unsigned char, int> symbolFrequencies;
    
    //calculate entropy
    cout<<"Entropy:\n"<<endl;
    cout<<calculateEntropy(data)<<endl;
    string encodedData;  // This will hold the encoded data
    for (unsigned char byte : data) {
        symbolFrequencies[byte]++;
    }
    //print the frequency information
    for ( auto &e: symbolFrequencies) {
        std::cout << "Key: " << e.first << ", Value: " << e.second << std::endl;
    }
    //sort symbols in descending order
    vector<pair<unsigned char, int> > sortedSymbols(symbolFrequencies.begin(), symbolFrequencies.end());
    //sort symbols with its frequency
    sort(sortedSymbols.begin(), sortedSymbols.end(),compareSymbols);
    //symbols in original file
    std::vector<char> arr;
    //The frequency of symbols in original file
    std::vector<int> frequency;
    //symbols and corresponding codeword;
    map<char, string> codeword;
    std::unordered_map<int,std::vector<int> > codebook;
    
    for(auto e:sortedSymbols){
        arr.push_back(e.first);
        frequency.push_back(e.second);
    }
    if(arr.size()==frequency.size()){
        HuffmanCodes(arr, frequency, arr.size(),codeword);
    }

    for(auto e:codeword){
        std::vector<int> bits;

        for (auto num:e.second){
            //num is char, bit is int
            int bit=num-48;
            bits.push_back(bit);
        }
        codebook[int(e.first)]=bits;
    }
    std::cout<<"codebook generated"<<std::endl;
    return codebook;
}
//===============================//
//QM coder with direct mapping
//input: path of input file
//output: file name of output file
//functionPtr: function pointer of byte2binary functions
void encodeDirect(const char* input,const char* output,std::bitset<8> (*functionPtr)(int)){
    int cntsymbol=0;
    int c=0;
    FILE *fp=fopen(input, "rb");
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
 do{
    //chars are converted to int base on ASCII
    c = fgetc(fp);
    cntsymbol++;
    //cout<<c<<endl;

    if (c != EOF)
    {
        std::bitset<8> out8bits= functionPtr(c); //your own code converting a byte to 8 bits
        for (int i = 0; i < out8bits.size(); i++)
        {
            qmcoder.encode(out8bits[i], 0);//BAC coding
            // cout<<out8bits[i]<<endl;
        }
    }
    } while (c != EOF);
    std::cout<<cntsymbol<<std::endl;
    fclose(op);
    qmcoder.reset();

}
//Qm coder with huffman mapping
//input: path of input file
//output: file name of output file
//codebook: huffman coding codebook
void encodehuff(const char* input,const char* output,std::unordered_map<int,std::vector<int> > codebook){
    int cntsymbol=0;
    int c=0;
    FILE *fp=fopen(input, "rb");
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
 do{
    //chars are converted to int base on ASCII
    c = fgetc(fp);
    cntsymbol++;
    //cout<<c<<endl;
    
    if (c != EOF)
    {
        vector<int> outbits=codebook[c]; //your own code converting a byte to 8 bits
        for (int i = 0; i < outbits.size(); i++)
        {
            qmcoder.encode(outbits[i], 0);//BAC coding
            // cout<<outbits[i]<<endl;
        }
    }
    } while (c != EOF);
    fclose(op);
    qmcoder.reset();
    cout<<"encode successful"<<endl;

}

//QM coder with bit plant encoding
void encodeBitPlane(const char* input,const char* output){
    int cntsymbol=0;
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
    for(int i=0;i<8;i++){
        int c=0;
        FILE *fp=fopen(input, "rb");
        do{
            //chars are converted to int base on ASCII
            c = fgetc(fp);
            cntsymbol++;
            //cout<<c<<endl;
            
                if (c != EOF)
                {
                    if (c >= 0 && c <= 255) {
                        std::bitset<8> binary(c);
                        qmcoder.encode(binary[i], 0);//BAC coding
                        // cout<<binary[i]<<endl;
                    }
                }
            
            
            } while (c != EOF);
            fclose(fp);

    }
    cout<<cntsymbol<<endl;
    fclose(op);
    qmcoder.reset();
    qmcoder.Flush();
    

}
int main(){
    //Encode the four media files using the QMcoder with direct convert
    //encodeDirect("image.dat","imageoutput",byte2binaryDriect);
    // encodeDirect("audio.dat","audiooutput",byte2binaryDriect);
    // encodeDirect("text.dat","textoutput",byte2binaryDriect);
    // encodeDirect("binary.dat","binaryoutput",byte2binaryDriect);
    
    encodeBitPlane("image.dat","imageoutputBitPlane");
    encodeBitPlane("audio.dat","audiooutputBitPlane");
    encodeBitPlane("text.dat","textoutputBitPlane");
    encodeBitPlane("binary.dat","binaryoutputBitPlane");
    // encodehuff("image.dat","imageoutputHuffman",huffmanMapping("image.dat"));
    // encodehuff("audio.dat","audiooutputHuffman",huffmanMapping("audio.dat"));
    // encodehuff("text.dat","textoutputHuffman",huffmanMapping("text.dat"));
    // encodehuff("binary.dat","binaryoutputHuffman",huffmanMapping("binary.dat"));


}