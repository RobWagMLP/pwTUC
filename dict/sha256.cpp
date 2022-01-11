#include <sha256.h>

bool compareSha(uint8_t *input, uint32_t length, uint32_t (&pwNrs)[8]) {
    uint32_t sha[8] = {};
    sha256(input, length, sha);
    //cout << sha[0] <<"\n";
    return sha[0] == pwNrs[0] && sha[1] == pwNrs[1] && sha[2] == pwNrs[2] && sha[3] == pwNrs[3] &&
           sha[4] == pwNrs[4] && sha[5] == pwNrs[5] && sha[6] == pwNrs[6] && sha[7] == pwNrs[7] ;
}

string shaString(uint8_t *input, int length) {
    uint32_t sha[8] = {};
    sha256(input, length, sha);
    stringstream result;
    result << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[0]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[1]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[2]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[3]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[4]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[5]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[6]
           << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)  << std::hex << sha[7];
    return result.str();
}

uint32_t* sha256(uint8_t *input, uint32_t inpLength, uint32_t (&fill)[8]) {

    //def init vecs
    uint32_t cA = 0x6a09e667;
    uint32_t cB = 0xbb67ae85;
    uint32_t cC = 0x3c6ef372;
    uint32_t cD = 0xa54ff53a;
    uint32_t cE = 0x510e527f;
    uint32_t cF = 0x9b05688c;
    uint32_t cG = 0x1f83d9ab;
    uint32_t cH = 0x5be0cd19;

    size_t length = 8*inpLength;
    //fill up to multiple of 512
    // since we want to crack passwords, it doesnt make sense to take care of cases with length of multiples of 512, We couldnt crack that anyways and so we can safe some performance
    uint64_t dif = ((448 - length%448)/8) - 1;
    int bitLength = 512/8;
    uint8_t *bits = new uint8_t[bitLength]{0};
    for(size_t i = 0; i < inpLength; i++) {
        bits[i] = input[i];
    }
    bits[inpLength] = 128;

    for(size_t i =0; i < dif; i++) {
        bits[i+1+inpLength] = 0;
    }
    uint32_t charLength = 8*inpLength;

    //add length as 4*8Bit integer
    bits[bitLength - 4] = (charLength >> 24) & 0xff;
    bits[bitLength - 3] = (charLength >> 16) & 0xff;
    bits[bitLength - 2] = (charLength >>  8) & 0xff;
    bits[bitLength - 1] = (charLength      ) & 0xff;

    //iterate over blocks

    for (size_t block = 0; block < bitLength; block = block + 512) {
        uint32_t A = cA;
        uint32_t B = cB;
        uint32_t C = cC;
        uint32_t D = cD;
        uint32_t E = cE;
        uint32_t F = cF;
        uint32_t G = cG;
        uint32_t H = cH;

        //define value to replace %2^32 with &(2^32 -1), performance matters!
        uint32_t maxBits = (0x100000000 - 1);
        //split bit 512 bit blocks into 16 32-bit numbers
        uint32_t m[64] = {};
        for(size_t mI = 0; mI < 16; mI++) {
            m[mI] = (((uint32_t) bits[4*mI]) << 24) + (((uint32_t) bits[4*mI + 1]) << 16) + (((uint32_t) bits[4*mI + 2]) << 8) + ((uint32_t) bits[4*mI +3]);
        }
        for(size_t vN = 16;  vN < 64; vN++) {
            uint32_t s0 = Frot(m[vN - 15], 7, 18, 3);
            uint32_t s1 = Frot(m[vN - 2], 17, 19, 10);
            m[vN] = mA(m[vN-16], s0, m[vN-7], s1, maxBits);
        }
        for (size_t round = 0; round < 64; round++) {
             uint32_t t1 = (H + (Grot(E, 6, 11, 25)) + ((E & F) ^ ((~E) & G)) + K[round] + m[round])&maxBits;
             uint32_t ma = (A & B) ^ (A & C) ^ (B & C);

             H = G; G = F;  F = E; E = (D + t1)&maxBits; D = C;  C = B;  B = A; A = (t1 + ((Grot(A, 2, 13, 22) + ma)&maxBits))&maxBits;
        }
        cA = (cA + A)&maxBits; cB = (cB + B)&maxBits; cC = (cC + C)&maxBits; cD = (cD + D)&maxBits;
        cE = (cE + E)&maxBits; cF = (cF + F)&maxBits; cG = (cG + G)&maxBits; cH = (cH + H)&maxBits;
    }
    fill[0] = cA; fill[1] = cB; fill[2] = cC; fill[3] = cD; fill[4] = cE; fill[5] = cF; fill[6] = cG; fill[7] = cH;
    delete[] bits;
}

uint32_t rightRotate(uint32_t n, uint32_t d) {
   return (n >> d ) | (n << (32 - d));
}

uint32_t mA(uint32_t U, uint32_t V, uint32_t X, uint32_t Y, uint32_t Z) {
    return (U + V + X + Y)&Z;
}

uint32_t Frot(uint32_t B, uint32_t x, uint32_t y, uint32_t z) {
    return rightRotate(B, x) ^ rightRotate(B, y) ^ (B >> z);
}

uint32_t Grot(uint32_t B, uint32_t x, uint32_t y, uint32_t z) {
    return rightRotate(B, x) ^ rightRotate(B, y) ^ rightRotate(B, z);
}

