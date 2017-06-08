#include "BmCombineQuan.h"

BmCombineQuan::BmCombineQuan()
{
    //ctor
}

BmCombineQuan::~BmCombineQuan()
{
    //dtor
}

void BmCombineQuan::Embed(Block** block, int randomSeed){
    srand(randomSeed);

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            Bitmap* bitmap = block[block_h][block_w].GetBitmapData();
            int bm_value = 0;
            int q_a = block[block_h][block_w].q_a;
            int q_b = block[block_h][block_w].q_b;

            //若q_a = q_b，bm均為0，塞入資訊後有更動到q_a或q_b，在檢測時會出錯，故q_a = q_b時，直接將bm設成01交錯，q_a-mod或q_b+mod
            if(q_a == q_b){
                int r = 0;
                for(int bm_h = 0; bm_h < bitmap->bm_h; bm_h++){
                    for(int bm_w = 0; bm_w < bitmap->bm_w; bm_w++){
                        bitmap->bm[bm_h][bm_w] = r%2 == 0 ? false : true;
                        r++;
                    }
                }
                if(q_a - this->mod >= 0){
                    block[block_h][block_w].q_a = q_a = q_a - this->mod;
                }
                else if(q_b + this->mod <=255){
                    block[block_h][block_w].q_b = q_b = q_b + this->mod;
                }

            }

            for(int bm_h = 0; bm_h < bitmap->bm_h; bm_h++){
                for(int bm_w = 0; bm_w < bitmap->bm_w; bm_w+=2){
                    bm_value += ((int)bitmap->bm[bm_h][bm_w])*2 + (int)bitmap->bm[bm_h][bm_w+1];
                }
            }

            //避免調整時出錯
            if(q_a - this->mod < 0){
                block[block_h][block_w].q_a = q_a = q_a + this->mod;
                block[block_h][block_w].q_b = q_b = q_b + this->mod;
            }
            else if(q_b + this->mod > 255){
                block[block_h][block_w].q_a = q_a = q_a - this->mod;
                block[block_h][block_w].q_b = q_b = q_b - this->mod;
            }

            bm_value *= q_b;

            int random = rand();

            int qMod = std::abs(q_a-q_b)%this->mod;
            int randAuth = (bm_value*random)%this->mod;

            /*if(block_h == 120 && block_w == 92){
                std::cout << qMod << " " << randAuth << " " << std::abs(qMod - randAuth) << std::endl;
            }*/

            //this->AdjustQuan(block, block_h, block_w, randAuth, qMod, bm_value);

            int diff = std::abs(qMod - randAuth);

            if(randAuth != qMod){
                if(randAuth > qMod && q_a - diff >= 0){
                    block[block_h][block_w].q_a -= diff;
                }
                else if(randAuth < qMod && q_a + diff < q_b){
                    block[block_h][block_w].q_a += diff;
                }
                else if(randAuth < qMod && q_a + diff >= q_b){
                    block[block_h][block_w].q_a = (q_a - this->mod) + diff;
                }
            }

            bitmap = NULL;
        }
    }
}

void BmCombineQuan::AdjustQuan(Block** block, int h, int w, int randAuth, int qMod, int bm_value){
    int q_a = block[h][w].q_a;
    int q_b = block[h][w].q_b;

    int diff = std::abs(qMod - randAuth);

    if(randAuth != qMod){
        if(randAuth > qMod){
            if(q_a - diff >= 0){
                block[h][w].q_a -= diff;
            }

            else if(q_a - diff < 0){
                if((q_a + this->mod) - diff > q_b){
                    bool flag = q_a - 0 > 255 - q_a ? true : false; //判斷距離0較近還是255較近

                    bm_value /= q_b;
                    if(flag == true){
                        block[h][w].q_b = q_b = q_b + this->mod;
                        this->AdjustQuan(block, h, w, randAuth, qMod, bm_value*q_b);
                    }
                    else{
                        block[h][w].q_a = q_a = (q_a - this->mod - diff);
                    }
                }
                else{
                    block[h][w].q_a = (q_a + this->mod) - diff;
                }
            }
        }
        else if(randAuth < qMod && q_a + diff < q_b){
            block[h][w].q_a += diff;
        }
        else if(randAuth < qMod && q_a + diff >= q_b){
            if((q_a - this->mod) + diff >= q_b){
                bool flag = q_a - 0 > 255 - q_a ? true : false; //判斷距離0較近還是255較近

                if(flag == true){
                    bm_value /= q_b;
                    block[h][w].q_b = q_b = q_b + this->mod;
                    this->AdjustQuan(block, h, w, randAuth, qMod, bm_value*q_b);
                }
                else{
                    block[h][w].q_a = q_a = (q_a - this->mod - diff);
                }
            }
            else{
                block[h][w].q_a = (q_a - this->mod) + diff;
            }
        }
    }
}

void BmCombineQuan::Detected(Block** block, int randomSeed){
    srand(randomSeed);

    this->dirtyCount = 0;
    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            Bitmap* bitmap = block[block_h][block_w].GetBitmapData();
            int bm_value = 0;

            for(int bm_h = 0; bm_h < bitmap->bm_h; bm_h++){
                for(int bm_w = 0; bm_w < bitmap->bm_w; bm_w+=2){
                    bm_value += ((int)bitmap->bm[bm_h][bm_w])*2 + (int)bitmap->bm[bm_h][bm_w+1];
                }
            }

            bm_value *= block[block_h][block_w].q_b;

            int random = rand();
            int q_a = block[block_h][block_w].q_a;
            int q_b = block[block_h][block_w].q_b;

            if((bm_value*random)%this->mod != std::abs(q_a-q_b)%this->mod){
                block[block_h][block_w].dirty = true;
                this->dirtyCount++;
                std::cout << "Error at : " << block_h << " " << block_w << " " << std::endl;
            }

            bitmap = NULL;
        }
    }

    std::cout << "Roughly Detected : " << this->dirtyCount << std::endl;
}

void BmCombineQuan::Attack(Block** block){

    srand(1001);

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            int random_a = rand() % 256;
            int random_b = 0;

            int q_a = block[block_h][block_w].q_a;
            int q_b = block[block_h][block_w].q_b;

            int diff = std::abs(q_a - q_b);

            if(random_a + diff > 255){
                random_b = random_a;
                random_a -= diff;
            }
            else{
                random_b = random_a + diff;
            }

            block[block_h][block_w].q_a = random_a;
            block[block_h][block_w].q_b = random_b;
        }
    }
}

int BmCombineQuan::GetDirtyCount(){
    return this->dirtyCount;
}
