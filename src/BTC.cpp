#include <fstream>
#include <cmath>
#include <map>
#include <cstdlib>
#include <iomanip>
#include "BTC.h"

BTC::BTC(){  //initial
    this->orgin_Image = new unsigned char[H*W];
    this->change_Image = new unsigned char[H*W];
    this->block = new Block*[(H/b_h)];
    for(int i=0;i<(H/b_h);i++)this->block[i] = new Block[(W/b_w)];

    this->attack_Image = new unsigned char[H*W];
    this->attack_block = new Block*[(H/b_h)];
    for(int i=0;i<(H/b_h);i++)this->attack_block[i] = new Block[(W/b_w)];

    this->detected_Image = new unsigned char[H*W];
    for(int i=0;i<H*W;i++)this->detected_Image[i] = 255;

    this->detected_count = 0;
}

BTC::~BTC(){  //dispose
    delete [] this->orgin_Image;
    delete [] this->change_Image;
    delete [] this->block;
    delete [] this->attack_block;
    delete [] this->attack_Image;
    delete [] this->detected_Image;
}

void BTC::Input(char* file){
    std::fstream input;
    input.open(file,std::ios::in | std::ios::binary);
    if(!input){std::cout << "Open file failed." << std::endl; exit(EXIT_FAILURE);}
    std::cout << "Inputing..." << std::endl;
    input.read((char*)this->orgin_Image,H*W);
    std::cout << "Input successed." << std::endl;
    input.close();
}

void BTC::Ouput(char* file){
    std::fstream ouput;
    ouput.open(file,std::ios::out | std::ios::binary);
    if(!ouput){std::cout << "Open file failed." << std::endl; exit(EXIT_FAILURE);}
    std::cout << "Ouputing..." << std::endl;
    ouput.write((char*)this->change_Image,H*W);
    std::cout << "Ouput successed." << std::endl;
    ouput.close();
}

void BTC::Divide(){
    double avg = 0.0;
    for(int block_h=0,orgin_h=0;block_h<(H/b_h) && orgin_h<H;block_h++,orgin_h=orgin_h+b_h){
        for(int block_w=0,orgin_w=0;block_w<(W/b_w) && orgin_w<H;block_w++,orgin_w=orgin_w+b_w){
            for(int i=0;i<b_h;i++){
                for(int j=0;j<b_w;j++){
                    this->block[block_h][block_w].pixel_in_block[i][j] = orgin_Image[((orgin_h+i)*W)+(orgin_w+j)];
                    avg += this->block[block_h][block_w].pixel_in_block[i][j];
                }
            }

            avg /= b_h*b_w;
            this->block[block_h][block_w].ave_pix = avg;
            avg = 0.0;
        }
    }
}

void BTC::Compress(){
    std::cout << "Compressing...." << std::endl;
    int smal_avg = 0,larg_avg = 0;    //two variable values of smaller than average and larger than average.
    int q = 0;    //q is the number of larger than the average of pixels of the block.
    for(int block_h=0;block_h<(H/b_h);block_h++){
        for(int block_w=0;block_w<(W/b_w);block_w++){
            for(int p_h=0;p_h<b_h;p_h++){    //p_h is the index of the pixel in  block.
                for(int p_w=0;p_w<b_w;p_w++){    //p_w is the index of the pixel in block.
                    if((double)this->block[block_h][block_w].pixel_in_block[p_h][p_w] < this->block[block_h][block_w].ave_pix){
                        smal_avg += this->block[block_h][block_w].pixel_in_block[p_h][p_w];
                        this->block[block_h][block_w].bitmap->bm[p_h][p_w] = false;    //bitmap will be false if this pixel is smaller than average.
                    }
                    else{
                        larg_avg += this->block[block_h][block_w].pixel_in_block[p_h][p_w];
                        this->block[block_h][block_w].bitmap->bm[p_h][p_w] = true;    //bitmap will be false if this pixel is larger than average.
                        q++;
                    }
                }
            }
            this->block[block_h][block_w].q_a = 1.0/((b_h*b_w)-q)*smal_avg;    //compute the quantization a of the block.
            this->block[block_h][block_w].q_b = 1.0/q*larg_avg;    //compute the quantization b of the block.
            if(this->block[block_h][block_w].q_b == (int)this->block[block_h][block_w].ave_pix && this->block[block_h][block_w].q_a == 0){
                this->block[block_h][block_w].q_a = this->block[block_h][block_w].q_b;
            }
            smal_avg = 0;larg_avg = 0,q = 0;
        }
    }
    std::cout << "Compression has been finished." << std::endl;
}

void BTC::Decompress(){    //the method is similar as Divide.
    for(int p_h=0,block_h=0;p_h<H,block_h<(H/b_h);p_h=p_h+b_h,block_h++){
        for(int p_w=0,block_w=0;p_w<W,block_w<(W/b_w);p_w=p_w+b_w,block_w++){
            for(int bm_h=0;bm_h<b_h;bm_h++){
                for(int bm_w=0;bm_w<b_w;bm_w++){
                    this->change_Image[((p_h+bm_h)*W)+(p_w+bm_w)] = this->block[block_h][block_w].bitmap->bm[bm_h][bm_w] == true
                                                                  ? this->block[block_h][block_w].q_b
                                                                  : this->block[block_h][block_w].q_a;
                }
            }
        }
    }
}

void BTC::Show_block(int h,int w){    //show a content of a block.
    std::cout << "block:" << std::endl;
    if((h>=(H/b_h) || w>=(W/b_w)) || (h<0 || w<0)){
        std::cout << "Index error." << std::endl;
        exit(EXIT_FAILURE);
    }
    else{
        for(int i=0;i<b_h;i++){
            for(int j=0;j<b_w;j++){
                unsigned int pixel = this->block[h][w].pixel_in_block[i][j];
                std::cout << pixel << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "A:" << this->block[h][w].q_a << " " << "B:" << " " << this->block[h][w].q_b << std::endl;
        for(int i=0;i<b_h;i++){
            for(int j=0;j<b_w;j++){
                unsigned int pixel = this->block[h][w].bitmap->bm[i][j];
                std::cout << pixel << " ";
            }
            std::cout << std::endl;
        }
    }
    if(this->block[h][w].dirty == true)std::cout << "dirty" << std::endl;
}

double BTC::PSNR(){
    double MSE = 0.0,PSNR = 0.0;
    for(int h=0;h<H;h++){
        for(int w=0;w<W;w++){
            MSE += pow(this->orgin_Image[h*w+w]-this->change_Image[h*w+w],2);
        }

    }
    double m_2 = H*W;
    MSE = (1.0/m_2)*MSE;
    PSNR = 10*(log10(pow(255.0,2)/MSE));
    return PSNR;
}
