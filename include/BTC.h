#ifndef BTC_H
#define BTC_H

#include "Block.h"
#include <map>

const int H = 512;
const int W = 512;

class BTC{
    public:
        BTC();
        ~BTC();
        void Input(char *file);
        void Ouput(char *file);
        void Divide();
        void Compress();
        void Decompress();
        void BTC_Compress(char *file);
        double PSNR();
        void Show_block(const int h,const int w);

        Block** GetBlockData();
        unsigned char* GetOriginalImage();
        unsigned char* GetChangeImage();

    private:
        unsigned char* orgin_Image = NULL;
        unsigned char* change_Image = NULL;
        //unsigned char* detected_Image = NULL;
        Block** block = NULL;
        //unsigned char* attack_Image = NULL;
        //Block** attack_block = NULL;
        //Block** detected_block = NULL;
        //int authen_seed;
        //int quantization_seed;
        int len;
        //int detected_count;
        //random function: Xn = (aXn-1 + c)mod n
        //int a,c;
};

#endif // BTC_H
