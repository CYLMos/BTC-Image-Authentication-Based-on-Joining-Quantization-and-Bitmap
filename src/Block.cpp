#include "Block.h"

Block::Block(){
    this->ave_pix = 0.0;
    this->q_a = 0;
    this->q_b = 0;
    this->dirty = false;
    this->bitmap = new Bitmap(b_h,b_w);
    this->pixel_in_block = new unsigned char*[b_h];
    for(int i=0;i<b_h;i++){
        this->pixel_in_block[i] = new unsigned char[b_w];
    }
}

Block::~Block(){
    delete this->bitmap;
    delete [] this->pixel_in_block;
}

Bitmap* Block::GetBitmapData(){
    return this->bitmap;
}
