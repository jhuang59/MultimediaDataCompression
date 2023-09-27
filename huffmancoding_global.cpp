
//==================================//
//copied from https://cppsecrets.com/users/14739971109711010010097115971101054864103109971051084699111109/C00-Program-for-Huffman-Code.php
//Add Codeword()
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <fstream>
#include <cmath>
using namespace std;

struct MinHeapNode
{
    char d;
    unsigned frequency;
    MinHeapNode *lChild, *rChild;

    MinHeapNode(char d, unsigned frequency)

    {

        lChild = rChild = NULL;
        this->d = d;
        this->frequency = frequency;
    }
};
//function to compare
struct compare
{
    bool operator()(MinHeapNode *l, MinHeapNode *r)
    {
        return (l->frequency > r->frequency);
    }
};

void printCodes(struct MinHeapNode *root, string str)
{
    if (!root)
        return;

    if (root->d != '$')
        cout << root->d << ": " << str << "\n";

    printCodes(root->lChild, str + "0");
    printCodes(root->rChild, str + "1");
}
//Adding codewords to map<char, string> codeword recursively
void Codeword(struct MinHeapNode *root, string str,map<char, string> &codeword)
{
    if (!root)
        return;

    if (root->d != '$')
        codeword[root->d]=str;

    Codeword(root->lChild, str + "0",codeword);
    Codeword(root->rChild, str + "1",codeword);
}
void HuffmanCodes(std::vector<char> d, std::vector<int> frequency, int size,map<char, string> &codeword)
{
    struct MinHeapNode *lChild, *rChild, *top;

    priority_queue<MinHeapNode *, vector<MinHeapNode *>, compare> minHeap;

    for (int i = 0; i < size; ++i)
        minHeap.push(new MinHeapNode(d[i], frequency[i]));

    while (minHeap.size() != 1)
    {
        lChild = minHeap.top();
        minHeap.pop();
        rChild = minHeap.top();
        minHeap.pop();
        top = new MinHeapNode('$', lChild->frequency + rChild->frequency);
        top->lChild = lChild;
        top->rChild = rChild;

        minHeap.push(top);
    }
    cout<<"Symbols and corresponding code"<<endl;
    printCodes(minHeap.top(), "");
    Codeword(minHeap.top(), "",codeword);
    cout<<"Codeword recorded"<<endl;
}
//==================================//

//==================================//
//cpoied from chatgpt
//given vector of bytes, calculate the entropy
double calculateEntropy(const vector<unsigned char> &pixelData) {
    std::map<unsigned char, double> probabilityDistribution;

    // Calculate the probability distribution of pixel values
    for (auto pixel : pixelData) {
        probabilityDistribution[pixel]++;
    }

    // Normalize probabilities
    int totalPixels = pixelData.size();
    for (auto& pair : probabilityDistribution) {
        pair.second /= totalPixels;
    }

    // Calculate entropy
    double entropy = 0.0;
    for (const auto& pair : probabilityDistribution) {
        double probability = pair.second;
        entropy -= probability * log2(probability);
    }

    return entropy;
}
//==================================//
//==================================//
//Copy right: Jie Huang
//https://github.com/jhuang59/MultimediaDataCompression
//custom compare function
bool compareSymbols(const pair<unsigned char, int>& a, const pair<unsigned char, int>& b) {
    return a.second > b.second;
}
//Huffman encoder global statistics
//input:FilePath:File to encode
//input:outputfile:encoded file
int encoder(string FilePath,string outputfile){
    
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
    for(auto e:sortedSymbols){
        arr.push_back(e.first);
        frequency.push_back(e.second);
    }
    if(arr.size()==frequency.size()){
        HuffmanCodes(arr, frequency, arr.size(),codeword);
    }
    // for(auto e:codeword){
    //     cout<<e.first<<endl;
    // }
    for (unsigned char symbol : data) {
        // Find the code for the symbol in the codebook
        string code = codeword[symbol];
        encodedData += code;  // Append the code to the encoded data
    }
    //write bits onto output file
    ofstream outputFile(outputfile, ios::binary);
    int bitsToWrite = 0;
    unsigned char currentByte = 0;

    for (char bit : encodedData) {
        currentByte = (currentByte << 1) | (bit - '0');
        bitsToWrite++;

        if (bitsToWrite == 8) {
            outputFile.put(currentByte);
            bitsToWrite = 0;
            currentByte = 0;
        }
    }
    // If there are remaining bits, write them padded with zeros
    if (bitsToWrite > 0) {
        currentByte <<= (8 - bitsToWrite);
        outputFile.put(currentByte);
    }
    outputFile.close();
    cout<<"Encoded file:"<<outputfile<<endl;

}
//==================================//
// int main()
// {
    
//     encoder("text.dat","text.bin");
//     //encoder("audio.dat","audio.bin");
//     //encoder("image.dat","image.bin");
//     //encoder("binary.dat","binary.bin");
// }