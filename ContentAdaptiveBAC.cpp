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
//context: number of context bits
void encodeDirect(const char* input,const char* output,std::bitset<8> (*functionPtr)(int),int context){
    int cntsymbol=0;
    int c=0;
    FILE *fp=fopen(input, "rb");
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
    //context bits
    vector<unsigned char> BitsVector(context,0);
 do{
    //chars are converted to int base on ASCII
    c = fgetc(fp);
    cntsymbol++;
    if (c != EOF)
    {
        std::bitset<8> out8bits= functionPtr(c);
        for (int i = 0; i < out8bits.size(); i++){
            int contextParam=0;
            //convert binary onto int
            for (int j = 0; j <BitsVector.size(); j++) {
                contextParam += pow(2, i) * BitsVector[j];
            }
            qmcoder.encode(out8bits[i], contextParam);//BAC coding
            BitsVector.erase(BitsVector.begin());//remove the first bit and push current bit
            BitsVector.push_back(out8bits[i]);
            // cout<<out8bits[i]<<endl;
        }
        
    }
    } while (c != EOF);
    std::cout<<cntsymbol<<std::endl;
    fclose(op);
    qmcoder.reset();
    cout<<"successful"<<endl;
}

//Qm coder with huffman mapping
//input: path of input file
//output: file name of output file
//codebook: huffman coding codebook
//context: number of bits in context
void encodehuff(const char* input,const char* output,std::unordered_map<int,std::vector<int> > codebook,int context){
    int cntsymbol=0;
    int c=0;
    FILE *fp=fopen(input, "rb");
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
    //context bits
    vector<unsigned char> BitsVector(context,0);
 do{
    //chars are converted to int base on ASCII
    c = fgetc(fp);
    cntsymbol++;
    //cout<<c<<endl;
    
    if (c != EOF)
    {
        vector<int> outbits=codebook[c]; //your own code converting a byte to bits
        
        for (int i = 0; i < outbits.size(); i++)
        {
            int contextParam=0;
            //convert binary onto int
            for (int j = 0; j <BitsVector.size(); j++) {
                contextParam += pow(2, i) * BitsVector[j];
            }
            qmcoder.encode(outbits[i], contextParam);//BAC coding
            BitsVector.erase(BitsVector.begin());//remove the first bit and push current bit
            BitsVector.push_back(outbits[i]);
        }
    }
    } while (c != EOF);
    fclose(op);
    qmcoder.reset();
    cout<<"encode successful"<<endl;

}

//QM coder with bit plant encoding
//input: path of input file
//output: file name of output file
//context: number of bits in context
void encodeBitPlane(const char* input,const char* output,int context){
    unordered_map<int,QM> map;
    int cntsymbol=0;
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
    //context bits, zero pading
    vector<unsigned char> BitsVector(context,0);
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
                        int contextParam=0;
                        //convert binary onto int
                        for (int j = 0; j <BitsVector.size(); j++) {
                            contextParam += pow(2, i) * BitsVector[j];
                        }
                        qmcoder.encode(binary[i], contextParam);//BAC coding
                        BitsVector.erase(BitsVector.begin());//remove the first bit and push current bit
                        BitsVector.push_back(binary[i]);
                        // cout<<binary[i]<<endl;
                    }
                }
            
            
            } while (c != EOF);
            fclose(fp);

    }
    //cout<<cntsymbol<<endl;
    fclose(op);
    qmcoder.reset();
    qmcoder.Flush();
    

}
//Divide image into blocks
//input:path of input file
//block:number of blocks in one dimension. the total number of blocks is block*block
vector<vector<vector<unsigned char> > > crop_image(const char* input,int block){
    ifstream data(input, ios_base::binary);
    vector<unsigned char> v((istreambuf_iterator<char>(data)), istreambuf_iterator<char>());
    int L=256;
    vector<vector<unsigned char> > image;
    vector<unsigned char> temp;
    
    vector<vector<unsigned char> > ImageBlock(L/block,vector<unsigned char>(block,0));
    
    for (int i=0;i<v.size();i++){
        if(temp.size()==L){
            image.push_back(temp);
            temp.clear();
        }
        temp.push_back(v[i]);
    }
    image.push_back(temp);
    vector<vector<vector<unsigned char> > > CropImage;
  
    for(int i=0;i<image.size();i++){
        for (int j=0;j<image.size();j++){
            ImageBlock[j/block][j%block]=image[i][j];
        }
        CropImage.push_back(ImageBlock);
        // ImageBlock.clear();
    }
    return CropImage;
}
//Qm coder with huffman mapping and crop preprocessing
//input: path of input file
//output: file name of output file
//codebook: huffman coding codebook
//context: number of bits in context
void encodehuffCrop(const char* input,const char* output,std::unordered_map<int,std::vector<int> > codebook,int context,int block){
    int cntsymbol=0;
    int c=0;
    vector<vector<vector<unsigned char> > > data=crop_image(input,block);
    // Create an ofstream object to write to the file
    FILE *op = fopen(output, "w");
    QM qmcoder(op);
    qmcoder.StartQM("encode");
    //context bits
    vector<unsigned char> BitsVector(context,0);
    for(int h=0;h<data.size();h++){
        for(int g=0;g<data[0].size();g++){
            for(int y=0;y<data[0][0].size();y++){
                //chars are converted to int base on ASCII
                c = data[h][g][y];
                cntsymbol++;
                //cout<<c<<endl;
    
            if (c != EOF)
            {
                vector<int> outbits=codebook[c]; //your own code converting a byte to bits
                
                for (int i = 0; i < outbits.size(); i++)
                {
                    int contextParam=0;
                    //convert binary onto int
                    for (int j = 0; j <BitsVector.size(); j++) {
                        contextParam += pow(2, i) * BitsVector[j];
                    }
                    qmcoder.encode(outbits[i], contextParam);//BAC coding
                    BitsVector.erase(BitsVector.begin());//remove the first bit and push current bit
                    BitsVector.push_back(outbits[i]);
                }
            }
            } 
        }
    }
    
    fclose(op);
    qmcoder.reset();
    cout<<"encode successful"<<endl;

}

int main(){
    std::vector<std::string> inputName;
    inputName.push_back("image");
    inputName.push_back("audio");
    inputName.push_back("text");
    inputName.push_back("binary");
    //filesystem is not avaliable in my local machine
    for (int i=1;i<=3;i++){
        for (auto name:inputName){
        std::string output = "output/"+to_string(i)+"/"+name+"BitPlaneCABAC"; // Convert int to string
        const char* outputchar = output.c_str();
        std::string input = "input/"+name+".dat"; // Convert int to string
        const char* inputchar = input.c_str();
        encodeBitPlane(inputchar,outputchar,i);
         }
    }
    for (int i=1;i<=3;i++){
        for (auto name:inputName){
        std::string output = "output/"+to_string(i)+"/"+name+"DirectCABAC"; // Convert int to string
        const char* outputchar = output.c_str();
        std::string input = "input/"+name+".dat"; // Convert int to string
        const char* inputchar = input.c_str();
        encodeDirect(inputchar,outputchar,byte2binaryDriect,i);
        }
    }
    for (int i=1;i<=3;i++){
        for (auto name:inputName){
        std::string output = "output/"+to_string(i)+"/"+name+"huffmanCABAC"; // Convert int to string
        const char* outputchar = output.c_str();
        std::string input = "input/"+name+".dat"; // Convert int to string
        const char* inputchar = input.c_str();
        encodehuff(inputchar,outputchar,huffmanMapping(input),i);
        }
    }
    
        
        //encodeBitPlane("input/image.dat","output/imageDirectCABAC",2);
        // encodeDirect("input/audio.dat","output/audioDirectCABAC",byte2binaryDriect,i);
        // encodeDirect("input/text.dat","output/textDirectCABAC",byte2binaryDriect,i);
        // encodeDirect("input/binary.dat","output/binaryDirectCABAC",byte2binaryDriect,i);
        //encodehuff("input/image.dat","output/imageHuffmanCABAC",huffmanMapping("input/image.dat"),3);
        // encodehuff("input/audio.dat","output/audioHuffmanCABAC",huffmanMapping("input/audio.dat"),i);
        // encodehuff("input/text.dat","output/textHuffmanCABAC",huffmanMapping("input/text.dat"),i);
        // //crop_image("input/text.dat",4);
        //encodehuff("input/binary.dat","output/binaryHuffmanCABAC",huffmanMapping("input/binary.dat"),4);
        //the number of blocks need to be divide exactly without remainder. eg.256%4==0
       //encodehuffCrop("input/image.dat","output/imageHuffmanCABACCrop",huffmanMapping("input/image.dat"),3,32);
    

}