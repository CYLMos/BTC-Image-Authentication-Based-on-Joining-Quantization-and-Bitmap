#include "QuanCombineBm.h"

QuanCombineBm::QuanCombineBm()
{
    //ctor
}

QuanCombineBm::QuanCombineBm(int len, int seed)
{
    this->len = len;
    this->bm_seed = seed;
}

QuanCombineBm::~QuanCombineBm()
{
    //dtor
}

void QuanCombineBm::SetLenValue(int len){
    this->len = len;
}

void QuanCombineBm::SetBmSeedValue(int seed){
    this->bm_seed = seed;
}

void QuanCombineBm::Embed(Block** block, int randomSeed){
    this->CodeGeneration(block,randomSeed);
    this->BmDivideAndEnbed(block);
}

void QuanCombineBm::CodeGeneration(Block** block, int randomSeed){
    srand(randomSeed);

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            int q_a = block[block_h][block_w].q_a;
            int q_b = block[block_h][block_w].q_b;
            if(q_a == q_b){
                q_a = block[block_h][block_w].q_a = q_a - 1;
            }
            int random = rand() * q_a * q_b;

            int ac = random % (int)pow(2,this->len);

            int* ac_binary = new int[this->len];

            for(int i = this->len - 1; i >= 0; i--){
                ac_binary[i] = ac % 2;
                ac /= 2;
            }

            Bitmap* bitmap = block[block_h][block_w].bitmap;
            bitmap->auth_bm = ac_binary;

            ac_binary = NULL;
            bitmap = NULL;
        }
    }
}

void QuanCombineBm::BmDivideAndEnbed(Block** block){
    srand(this->bm_seed);

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            int* sub_bm = new int[this->b_h*this->b_w];

            /** initial sub_bm **/
            for(int i = 0; i < this->b_h*this->b_w; i++){
                sub_bm[i] = i;
            }

            /** divide **/
            for(int a = 0; a < this->b_h*this->b_w; a++){
                int random = rand() % (this->b_h*this->b_w);
                int temp = sub_bm[a];
                sub_bm[a] = sub_bm[random];
                sub_bm[random] = temp;
            }

            Bitmap* bitmap = block[block_h][block_w].bitmap;

            this->EmbedInBm(bitmap, sub_bm);

            bitmap = NULL;
        }
    }
}

void QuanCombineBm::EmbedInBm(Bitmap* bitmap, int* classfication){
    bool** bm = bitmap->bm;
    int sub_num = (this->b_h*this->b_w)/this->len;
    int ac_index = 0;

    for(int class_index = 0; class_index < (this->b_h*this->b_w); class_index+=sub_num){
        int index[sub_num];
        for(int i = class_index, j = 0; i < class_index + sub_num && j < sub_num; i++, j++){
            index[j] = classfication[i];
        }

        int sub_bm_total = 0;
        for(int i = 0; i < sub_num; i++){
            int h = index[i]/this->b_h;
            int w = index[i]%this->b_w;

            sub_bm_total = bm[h][w] == true ? sub_bm_total + 1 : sub_bm_total;
        }

        bool parity_value = sub_bm_total % 2 == 1 ? true : false;
        bool sub_ac = bitmap->auth_bm[ac_index] == 1 ? true : false;

        if(sub_ac != parity_value){
            int max = 0;
            int victim_h = index[0]/this->b_h;
            int victim_w = index[0]%this->b_w;

            for(int sub_index = 0; sub_index < sub_num; sub_index++){
                int count = this->GetDifferentBoolCount(index[sub_index], sub_num, bm);
                if(count > max){
                    max = count;
                    victim_h = index[sub_index]/this->b_h;
                    victim_w = index[sub_index]%this->b_w;
                }
            }

            bitmap->bm[victim_h][victim_w] = bitmap->bm[victim_h][victim_w] == true ? false : true;
        }

        ac_index++;
    }

    bm = NULL;
}

int QuanCombineBm::GetDifferentBoolCount(int index, int sub_bm_num, bool** bm){
    int h = index/this->b_h, w = index%this->b_w;
    int count = 0;
    for(int bm_h = h-1; bm_h < h+2; bm_h++){
        for(int bm_w = w-1; bm_w < w+2; bm_w++){
            if( (bm_h<this->b_h && bm_h>-1) && (bm_w<this->b_w && bm_w>-1) ){
                count = bm[bm_h][bm_w] == bm[h][w] ? count+1 : count;
            }
        }
    }
    return count;
}

void QuanCombineBm::Detected(Block** block, int randomSeed){
    this->CodeGeneration(block, randomSeed);

    srand(this->bm_seed);

    this->dirtyCount = 0;

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            int* sub_bm = new int[this->b_h*this->b_w];
            bool dirtyFlag = false;

            /** initial sub_bm **/
            for(int i = 0; i < this->b_h*this->b_w; i++){
                sub_bm[i] = i;
            }

            /** divide **/
            for(int a = 0; a < this->b_h*this->b_w; a++){
                int random = rand() % (this->b_h*this->b_w);
                int temp = sub_bm[a];
                sub_bm[a] = sub_bm[random];
                sub_bm[random] = temp;
            }

            Bitmap* bitmap = block[block_h][block_w].bitmap;

            bool** bm = bitmap->bm;
            int sub_num = (this->b_h*this->b_w)/this->len;
            int ac_index = 0;

            /** Start Detected **/
            for(int class_index = 0; class_index < (this->b_h*this->b_w); class_index+=sub_num){
                int index[sub_num];
                for(int i = class_index, j = 0; i < class_index + sub_num && j < sub_num; i++, j++){
                    index[j] = sub_bm[i];
                }

                int sub_bm_total = 0;
                for(int i = 0; i < sub_num; i++){
                    int h = index[i]/this->b_h;
                    int w = index[i]%this->b_w;

                    sub_bm_total = bm[h][w] == true ? sub_bm_total + 1 : sub_bm_total;
                }

                bool parity_value = sub_bm_total % 2 == 1 ? true : false;
                bool sub_ac = bitmap->auth_bm[ac_index] == 1 ? true : false;

                if(sub_ac != parity_value){
                    block[block_h][block_w].dirty = true;
                    dirtyFlag = true;
                    //std::cout << "Error at: " << block_h << " " << block_w << std::endl;
                }

                ac_index++;
            }

            if(dirtyFlag == true){
                this->dirtyCount++;
                std::cout << "Error at: " << block_h << " " << block_w << std::endl;
            }

            bm = NULL;
            bitmap = NULL;
        }
    }

    std::cout << "Roughly Detected : " << this->dirtyCount << std::endl;
}

int QuanCombineBm::GetDirtyCount(){
    return this->dirtyCount;
}
