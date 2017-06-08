#include "Bitmap.h"
#include <cmath>

Bitmap::Bitmap(int h,int w){
    this->bm_h = h;this->bm_w = w;
    this->bm = new bool*[bm_h];
    for(int i=0;i<bm_h;i++){
        this->bm[i] = new bool[bm_w];
    }
}

Bitmap::~Bitmap(){
    delete [] this->bm;
    delete [] this->auth_bm;
    //delete this->parity_value;
}

/*void Bitmap::ac_generation(int len){
    this->auth_bm = new bool[len];

    int ac_decimal = this->random_num%(int)pow(2,len);
    for(int i=len-1;i>=0;i--){
        this->auth_bm[i] = ac_decimal%2;
        ac_decimal/=2;
    }
}

void Bitmap::ac_embed(int len){
    //****to know the size of the weight and the high of a bitmap****
    if(len == 2){
        this->sub_bm_h = 2;
        this->sub_bm_w = 4;
    }
    else if(len == 4){
        this->sub_bm_h = this->sub_bm_w = 2;
    }
    else if(len == 8){
        this->sub_bm_h = 1;
        this->sub_bm_w = 2;
    }
    else if(len == 16){
        this->sub_bm_h = this->sub_bm_w = 1;
    }
    else{
        this->sub_bm_h = this->sub_bm_w = 4;
    }

    //****embed ac****
    int pv_index = 0;
    for(int h=0;h<this->bm_h;h=h+this->sub_bm_h){
        for(int w=0;w<this->bm_w;w=w+this->sub_bm_w){

            int pv = 0;
            for(int sub_h=h;sub_h<this->sub_bm_h+h;sub_h++){
                for(int sub_w=w;sub_w<this->sub_bm_w+w;sub_w++){
                    pv = this->bm[sub_h][sub_w] == true ? pv+1 : pv;
                }
            }
            int flag = pv%2;
            if(flag != (int)this->auth_bm[pv_index]){
                this->change_count++;
                int victim_h = 0,victim_w = 0;
                int max_num = 0;
                for(int sub_h=h;sub_h<this->sub_bm_h+h;sub_h++){
                    for(int sub_w=w;sub_w<this->sub_bm_w+w;sub_w++){
                        int sub_bm_pix_count = 0;
                        //std::cout << "sub_w: "<< sub_w << std::endl;
                        for(int around_h=sub_h-1;around_h<(sub_h+2);around_h++){
                            for(int around_w=sub_w-1;around_w<(sub_w+2);around_w++){
                                if((around_h>=h && around_w>=w) && (around_h<h+this->sub_bm_h && around_w<w+this->sub_bm_w)){
                                    sub_bm_pix_count = this->bm[sub_h][sub_w] != this->bm[around_h][around_w] ? sub_bm_pix_count+1 : sub_bm_pix_count;
                                }
                            }
                        }
                        if(sub_bm_pix_count>max_num){
                            max_num = sub_bm_pix_count;
                            victim_h = sub_h;
                            victim_w = sub_w;
                        }
                        else if((sub_bm_pix_count == max_num) && (max_num == 0)){
                            victim_h = h;
                            victim_w = w;
                        }
                    }
                }
                this->bm[victim_h][victim_w] = this->bm[victim_h][victim_w] == true ? false : true;
            }
            pv_index++;

        }
    }
}

void Bitmap::ac_embed_improve(int len,int random_classfication[]){
    int sub_bm_num = (this->bm_h*this->bm_w)/len;
    bool dirty_group[len];
    for(int k=0;k<len;k++)dirty_group[k] = false;
    for(int group=0;group<len;group++){
        int total = 0;
        for(int num=0;num<sub_bm_num;num++){
            //std::cout << random_classfication[group*sub_bm_num+num] << std::endl;
            int index = random_classfication[group*sub_bm_num+num];
            total += (int)this->bm[index/this->bm_h][index%this->bm_w];
        }
        dirty_group[group] = total%2 != (int)this->auth_bm[group] ? true : false;
    }

    for(int group=0;group<len;group++){
        if(dirty_group[group] == true){
            int first_index = random_classfication[group*sub_bm_num+0];
            int victim_h = first_index/this->bm_h,victim_w = first_index%this->bm_w;
            int max_num = 0;
            for(int num=0;num<sub_bm_num;num++){
                int index = random_classfication[group*sub_bm_num+num];
                int count = this->get_different_bool_count(index,sub_bm_num);
                if(count > max_num){
                    max_num = count;
                    victim_h = index/this->bm_h;
                    victim_w = index%this->bm_w;
                }
            }
            this->bm[victim_h][victim_w] = this->bm[victim_h][victim_w] == true ? false : true;
            this->change_count++;
        }
    }
}

int Bitmap::get_different_bool_count(int index,int sub_bm_num){
    int h = index/this->bm_h, w = index%this->bm_w;
    int count = 0;
    for(int bm_h=h-1;bm_h<h+2;bm_h++){
        for(int bm_w=w-1;bm_w<w+2;bm_w++){
            if((bm_h<this->bm_h && bm_h>-1) && (bm_w<this->bm_w && bm_w>-1)){
                count = this->bm[bm_h][bm_w] == this->bm[h][w] ? count+1 : count;
            }
        }
    }
    return count;
}

bool Bitmap::bm_detected_improve(int len,int random_classfication[]){
    int sub_bm_num = (this->bm_h*this->bm_w)/len;
    for(int group=0;group<len;group++){
        int total = 0;
        for(int num=0;num<sub_bm_num;num++){
            int index = random_classfication[group*sub_bm_num+num];
            total += (int)this->bm[index/this->bm_h][index%this->bm_w];
        }
        if(total%2 != (int)this->auth_bm[group]){
            //std::cout << total << " " << *auth_bm << std::endl;
            return false;
        }
    }
    return true;
}

int Bitmap::getChangeCount(){
    return this->change_count;
}*/
