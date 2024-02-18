// Ryan Wu
// ID: weihuanw
// Carnegie Mellon University
// Eng. Computation, 24-780-B
// Prob. Set 2-2 Encryption & Decryption, Due Tues. Sept. 12, 2023


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

// Encryption
    // Setting up encryption files
void encryptFile(const std::string& inputFileName_encr, const std::string& outputFileName_encr, const std::string& outputFileCodeName_encr) {
    // Open input text file for encryption
    std::ifstream inputFile_encr(inputFileName_encr, std::ios::binary);
    inputFile_encr >> std::noskipws;
    if (!inputFile_encr) {
        printf("Error: failed to open your input text file for encryption.");
        return;
    }
    // Open output text file for encryption
    std::ofstream outputFile_encr(outputFileName_encr, std::ios::binary);
    if (!outputFile_encr) {
        printf("Error: failed to open your output text file for encryption.");
        inputFile_encr.close();
        return;
    }
    // Open output code file for encryption
    std::ofstream outputFileCode_encr(outputFileCodeName_encr, std::ios::binary);
    if (!outputFileCode_encr) {
        printf("Error: failed to open your output code file for encryption.");
        inputFile_encr.close();
        return;
    }

    // Encryption text file with random numbers 0-255
    unsigned char inEncrChar, outEncrChar, outEncrCode;
    while (!inputFile_encr.eof()) {
        inputFile_encr >> inEncrChar;
        unsigned char randomNumber = rand() % 255;
        outEncrChar = inEncrChar + randomNumber;
        outputFile_encr << outEncrChar;
        outEncrCode = randomNumber;
        outputFileCode_encr << outEncrCode;
    }
    
    //Closing encyption files
    inputFile_encr.close();
    outputFileCode_encr.close();
    outputFile_encr.close();
    printf("Your text file has been encrypted!\n");
}

// Decryption
   // Setting up decryption files
void decryptFile(const std::string& inputFileName_decr, const std::string& inputFileCodeName_decr, const std::string& outputFileName_decr) {
    // Open input text file for decryption
    std::ifstream inputFile_decr(inputFileName_decr, std::ios::binary);
    inputFile_decr >> std::noskipws;
    if (!inputFile_decr) {
        printf("Error: failed to open your input text file for decryption.");
        return;
    }
    // Open input code file for decryption
    std::ifstream inputFileCode_decr(inputFileCodeName_decr, std::ios::binary);
    inputFileCode_decr >> std::noskipws;
    if (!inputFileCode_decr) {
        printf("Error: failed to open your input code file for decryption.");
        return;
    }
    // Open output text file for decryption
    std::ofstream outputFile_decr(outputFileName_decr, std::ios::binary);
    if (!outputFile_decr) {
        printf("Error: failed to open your output text file for decryption.");
        inputFile_decr.close();
        inputFileCode_decr.close();
        return;
    }
    
    // Decrypting text file with random numbers 0-255
    unsigned char inDecrChar, inDecrCode, outDecrChar;
    while (!inputFile_decr.eof()) {
        inputFile_decr >> inDecrChar;
        inputFileCode_decr >> inDecrCode;
        outDecrChar = inDecrChar - inDecrCode;
        outputFile_decr << outDecrChar;
    }
    
    //Closing decryption files
    inputFile_decr.close();
    inputFileCode_decr.close();
    outputFile_decr.close();
    printf("Your text file has been decrypted!\n");
}

// File functions with user imput feature
int main() {
    
    // User input command for encryption, decryption or exit
    char userInput;
    std::cout << "Please indicate what action you want to take.\nEnter E for encryption, D for decryption, X to exit: ";
    std::cin >> userInput;
    std::string inputFileName_encr, outputFileName_encr, outputFileCode_encr, inputFileName_decr, inputFileCodeName_decr,outputFileName_decr;
    
    switch (userInput) {
        case 'E':
        case 'e':
            // File functions for encryption
            std::cout << "Please type input file's name for encryption: \n";
            std::cin >> inputFileName_encr;
            
            std::cout << "Please type output file's name for encryption: ";
            std::cin >> outputFileName_encr;
            
            std::cout <<"Please type output code file's name for encryption: ";
            std::cin >> outputFileCode_encr;
            encryptFile(inputFileName_encr, outputFileName_encr, outputFileCode_encr);
            break;
        case 'D':
        case 'd':
            // File functions for decryption
            std::cout << "Please type input file's name for decryption: ";
            std::cin >> inputFileName_decr;
            
            std::cout <<"Please type input code file's name for decryption: ";
            std::cin >> inputFileCodeName_decr;
            
            std::cout <<"Please type output file's name for decryption: ";
            std::cin >> outputFileName_decr;
            decryptFile(inputFileName_decr, inputFileCodeName_decr, outputFileName_decr);
        default:
            std::cout << "Goodbye dear friend!\n";
            return 0;
    }
}
