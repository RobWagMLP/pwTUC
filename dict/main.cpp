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

string password = "3ee8e7b83ba29a7f3079ff4fb0cdeca62d6e21acfe5466b8dc0b254c714c0a0d";

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
        uint8_t *chars = new uint8_t[length]{0};
        for(int i = 0; i < length ; i++) {
             chars[i] = resolver[values[i] ];
        }
        if(compareSha(chars,length, hashNr ) == true) {
            cout <<"Found! Password is: ";
            for(size_t i = 0; i < length; i++) {
                cout << chars[i];
            }
            cout << "\n";
            cout <<"Done in " << (std::time(0) - time) << " seconds";
            return;
        }
        if(values[0] != currenstart) {
            cout << "current letter: " << resolver[values[0]] <<" \n";
            currenstart = values[0];
        }
        delete[] chars;
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
