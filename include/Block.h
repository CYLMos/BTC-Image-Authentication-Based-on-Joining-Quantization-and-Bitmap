#include <iostream>

#ifndef BLOCK_H
#define BLOCK_H

#include "Bitmap.h"

const int b_h = 4;    //the size of the block
const int b_w = 4;    //...

class Block{
   public:
        Block();
        ~Block();
        double ave_pix;    //the average of pixels of the block.
        unsigned int q_a;    //quantization a.
        unsigned int q_b;    //quantization b.
        //bool** bm = NULL;    //bitmap
        Bitmap* bitmap = NULL;
        unsigned char** pixel_in_block = NULL;    //pixels in the block.
        bool dirty;

        Bitmap* GetBitmapData();
};

#endif // BLOCK_H
