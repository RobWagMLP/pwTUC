#include <iostream>
#include <math.h>
#include <fstream>
#include <string.h>
#include <bitset>
#include <sha256.h>
#include <charsets.h>

using namespace std;

void generate(uint8_t charset, uint8_t length);
int getCharsetLength(uint8_t charset);

const uint8_t* getResolver(uint8_t charset);

string password = "1f40bc4d52a194a5faa71e00707cbafdcc94b98bb899f1cb5b6f10b51acd9623";

int main(int argc, char** argv)
{
    int charset = 0;
    int wordlength = 4;
    for (int i = 1; i < argc; i = i +2) {
        if(strncmp(argv[i], "-c", 3 ) == 0) {
            charset = atoi(argv[i + 1]);
        } else if (strncmp(argv[i], "-w", 3 ) == 0) {
            wordlength = atoi(argv[i + 1]);
        } else if (strncmp(argv[i], "-p", 3 ) == 0) {
            password = argv[i+1];
        }
    }
    generate((uint8_t) charset, (uint8_t) wordlength);
}

void generate(uint8_t charset, uint8_t length) {
    time_t time = std::time(0);
    uint8_t *values = new uint8_t[length]{0};
    uint8_t currentIndex = length - 1;

    int charsetlength = getCharsetLength(charset);
    unsigned long long iterations = pow(charsetlength, length);
    uint32_t hashNr[8] = {};
    stringstream collector;
    int k = 0;
    for(size_t i = 0; i < password.length(); i++) {
        collector << password[i];
        if((i+1)%8 == 0) {
            hashNr[k] = (std::stoul(collector.str(), nullptr, 16));
            collector.clear();
            collector.str("");
            ++k;
        }
    }

    const uint8_t* resolver = getResolver(charset);
    uint8_t currenstart = values[0];

    for(unsigned long long i = 0; i < iterations; i++) {
        //generate new word
        while(values[currentIndex] == charsetlength - 1) {
               values[currentIndex] = 0;
               currentIndex--;
        }
        values[currentIndex]++;
        currentIndex = length - 1;

        //check password
        stringstream str;
        for(int i = 0; i < length ; i++) {
             str << resolver[values[i] ];
        }
        if(compareSha(str.str(), hashNr ) == true) {
            cout <<"Found! Password is: " << str.str() << "\n";
            cout <<"Done in " << (std::time(0) - time) << " seconds";
            return;
        }
        if(values[0] != currenstart) {
            cout << "current letter: " << resolver[values[0]] <<" \n";
            currenstart = values[0];
        }
    }
    cout << "Done, generated " << iterations << " entrys.\n";
    delete[] values;
}

int getCharsetLength(uint8_t charset) {
    switch(charset) {
        case 0: return 26;
        case 1: return 52;
        case 2: return 36;
        case 3: return 62;
        default: return 26;
    }
}

const uint8_t* getResolver(uint8_t charset) {
    switch(charset) {
        case 0: return lowerCh;
        case 1: return upperCh;
        case 2: return lowernumCh;
        case 3: return numCh;
        default: return lowerCh;
    }
}
