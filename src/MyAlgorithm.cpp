#include "MyAlgorithm.h"
#include <fstream>
#include <cstdlib>

MyAlgorithm::MyAlgorithm()
{
    this->method = NULL;
    this->block = NULL;
    this->randomSeed = 1000;
}

MyAlgorithm::~MyAlgorithm()
{
    delete this->method;
    delete this->block;
}

void MyAlgorithm::SetMethod(MyAlgorithmMethod<Block>* method)
{
    this->method = method;
}

void MyAlgorithm::SetBlockData(Block** block)
{
    this->block = block;
}

void MyAlgorithm::StartEmbed(){
    this->method->Embed(this->block,this->randomSeed);
}

void MyAlgorithm::StartDetected(){
    this->method->Detected(this->block,this->randomSeed);
    this->totalDirtyCount = this->method->GetDirtyCount();
    this->RefinementDetected();
}

void MyAlgorithm::RefinementDetected(){
    int count = 0;
    for(int block_h=0;block_h<(H/b_h);block_h++){
        for(int block_w=0;block_w<(W/b_w);block_w++){
            if(this->block[block_h][block_w].dirty == false){
                int detected_a_h = block_h - 1,detected_a_w = block_w - 1;
                int detected_b_h = block_h + 1,detected_b_w = block_w + 1;

                bool flag = false;
                while((detected_a_w < block_w+2) && (detected_b_w > block_w-2)){
                    if(detected_a_h>-1 && detected_a_w>-1 && detected_a_w<(W/b_w) && detected_b_h<(H/b_h) && detected_b_w<(W/b_w) && detected_b_w>-1){
                        if(this->block[detected_a_h][detected_a_w].dirty == true && this->block[detected_b_h][detected_b_w].dirty == true){
                            if(this->block[block_h][block_w].dirty == false){
                                this->totalDirtyCount++;
                            }
                            this->block[block_h][block_w].dirty = true;
                            count++;
                            flag = true;
                            break;
                        }
                    }
                    detected_a_w++;detected_b_w--;
                }
                detected_a_h++;detected_b_w--;
                if(flag != true){
                    if(detected_a_h>-1 && detected_a_w>-1 && detected_b_h<(H/b_h) && detected_b_w<(W/b_w)){
                        if(this->block[detected_a_h][detected_a_w].dirty == true && this->block[detected_b_h][detected_b_w].dirty == true){
                            if(this->block[block_h][block_w].dirty == false){
                                this->totalDirtyCount++;
                            }
                            this->block[block_h][block_w].dirty = true;
                            count++;
                        }
                    }
                }
            }
        }
    }
    std::cout << "refinement detected :" << count << std::endl;
}

void MyAlgorithm::OutPutDetected(char* file){
    unsigned char* detected_Image = new unsigned char[H*W];
    for(int i=0;i<H*W;i++){
        detected_Image[i] = 255;
    }

    for(int p_h=0,block_h=0;p_h<H,block_h<(H/b_h);p_h=p_h+b_h,block_h++){
        for(int p_w=0,block_w=0;p_w<W,block_w<(W/b_w);p_w=p_w+b_w,block_w++){
            if(this->block[block_h][block_w].dirty == true){
                for(int bm_h=0;bm_h<b_h;bm_h++){
                    for(int bm_w=0;bm_w<b_w;bm_w++){
                         detected_Image[((p_h+bm_h)*W)+(p_w+bm_w)] = 0;
                    }
                }
            }
        }
    }

    std::fstream ouput;
    ouput.open(file,std::ios::out | std::ios::binary);
    if(!ouput){std::cout << "Open file failed." << std::endl; exit(EXIT_FAILURE);}
    std::cout << "Ouputing..." << std::endl;
    ouput.write((char*)detected_Image,H*W);
    std::cout << "Ouput successed." << std::endl;
    ouput.close();

    delete detected_Image;
}

void MyAlgorithm::AttackImage(unsigned char* imageData, unsigned char* attackData){
    for(int h = 0; h < H; h++){
        for(int w = 0; w < W; w++){
            if(attackData[W*h+w] != 255){
                imageData[W*h+w] = attackData[W*h+w];
            }
        }
    }
}

int MyAlgorithm::GetTotalDirtyCount(){
    return this->totalDirtyCount;
}
