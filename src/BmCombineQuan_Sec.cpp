#include "BmCombineQuan_Sec.h"

BmCombineQuan_Sec::BmCombineQuan_Sec()
{
    //ctor
}

BmCombineQuan_Sec::~BmCombineQuan_Sec()
{
    //dtor
}

void BmCombineQuan_Sec::Embed(Block** block, int randomSeed){
    srand(randomSeed);

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
                //std::cout << block_h << " " << block_w << std::endl;
            Bitmap* bitmap = block[block_h][block_w].GetBitmapData();
            int bm_value = 0;
            int random = rand();
            int q_a = block[block_h][block_w].q_a;
            int q_b = block[block_h][block_w].q_b;

            /**若q_a = q_b，bm均為0，塞入資訊後有更動到q_a或q_b，在檢測時會出錯，故q_a = q_b時，直接將bm設成01交錯，q_a-mod或q_b+mod**/
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

            /**取得量化階a,b 的 MSB**/
            int binary_count = this->GetBinaryCount(this->mod);
            bool odd_or_even = binary_count % 2 == 1 ? true : false;

            int lsb_a_count = 0;
            int lsb_b_count = 0;

            if(odd_or_even == true){
                binary_count++;
                lsb_a_count = binary_count/2;
                lsb_b_count = binary_count/2;
            }
            else{
                lsb_a_count = binary_count/2;
                lsb_b_count = binary_count/2;
            }

            int* binary_a_array = new int[8];
            int* binary_b_array = new int[8];

            this->GetBinaryNum(binary_a_array, q_a, 8);
            this->GetBinaryNum(binary_b_array, q_b, 8);

            int msb_a_decimal = GetDecimalNum(binary_a_array, 8 - lsb_a_count, 8);
            int msb_b_decimal = GetDecimalNum(binary_b_array, 8 - lsb_b_count, 8);

            int hash_value = this->HashFunction(bm_value,random,msb_a_decimal,msb_b_decimal);
            int remainder = hash_value % this->mod;

            int* binary_code = new int[binary_count];
            this->GetBinaryNum(binary_code, remainder, binary_count);

            int* a_code = new int[lsb_a_count];
            int* b_code = new int[lsb_b_count];

            for(int i = 0; i < binary_count; i++){
                if(i < lsb_a_count){
                    a_code[i] = binary_code[i];
                }
                else{
                    b_code[i - lsb_a_count] = binary_code[i];
                }
            }

            int a_code_decimal = this->GetDecimalNum(a_code, lsb_a_count);
            int b_code_decimal = this->GetDecimalNum(b_code, lsb_b_count);

            delete a_code;
            delete b_code;
            delete binary_code;

            //std::cout << block_h << " " << block_w << std::endl;

            if(msb_a_decimal + a_code_decimal >= msb_b_decimal + b_code_decimal){

                a_code = new int[lsb_a_count];
                b_code = new int[lsb_b_count];
                binary_code = new int[binary_count];

                int change_value = pow(2, lsb_b_count);

                if(msb_b_decimal + b_code_decimal + change_value <= 255){
                    msb_b_decimal = msb_b_decimal + change_value;
                }
                else{
                    change_value = pow(2, lsb_a_count);
                    msb_a_decimal = msb_a_decimal - change_value;
                }

                hash_value = this->HashFunction(bm_value,random,msb_a_decimal,msb_b_decimal);//bm_value*random*(msb_a_decimal + msb_b_decimal);
                remainder = hash_value % this->mod;

                this->GetBinaryNum(binary_code, remainder, binary_count);

                for(int i = 0; i < binary_count; i++){
                    if(i < lsb_a_count){
                        a_code[i] = binary_code[i];
                    }
                    else{
                        b_code[i - lsb_a_count] = binary_code[i];
                    }
                }

                a_code_decimal = this->GetDecimalNum(a_code, lsb_a_count);
                b_code_decimal = this->GetDecimalNum(b_code, lsb_b_count);

                delete a_code;
                delete b_code;
                delete binary_code;
            }

            block[block_h][block_w].q_a = q_a = msb_a_decimal + a_code_decimal;
            block[block_h][block_w].q_b = q_b = msb_b_decimal + b_code_decimal;

            bitmap = NULL;
        }
    }
}

int BmCombineQuan_Sec::HashFunction(int bm_value, int random, int msb_a, int msb_b){
    std::hash<std::string> myHash;
    std::stringstream ss;

    ss << bm_value << random << msb_a << msb_b;

    int value = std::abs(myHash(ss.str()));
    return value;
}

int BmCombineQuan_Sec::GetDecimalNum(int* num_array , int size){
    int len = 0;
    int n = pow(2, len);
    int ans = 0;
    for(int i = size - 1; i >= 0; i--){
        ans += num_array[i] * n;
        len++;
        n = pow(2, len);
    }

    return ans;
}

int BmCombineQuan_Sec::GetDecimalNum(int* num_array, int index, int size){
    int len = 7;
    int n = pow(2, len);
    int ans = 0;
    for(int i = 0; i < index; i++){
        ans += num_array[i] * n;
        len--;
        n = pow(2, len);
    }

    return ans;
}

void BmCombineQuan_Sec::GetBinaryNum(int* num_array, int num, int size){
    for(int i = size - 1; i >= 0; i--){
        num_array[i] = num % 2;
        num /= 2;
    }
}

int BmCombineQuan_Sec::GetBinaryCount(int num){
    int count = 0;
    while(num != 0){
        if(num >= 2){
            count++;
            num /= 2;
        }
        else{
            break;
        }
    }

    return count;
}

void BmCombineQuan_Sec::Detected(Block** block, int randomSeed){
    srand(randomSeed);

    for(int block_h = 0; block_h < (this->H)/(this->b_h); block_h++){
        for(int block_w = 0; block_w < (this->W)/(this->b_w); block_w++){
            Bitmap* bitmap = block[block_h][block_w].GetBitmapData();
            int random = rand();
            int bm_value = 0;
            int q_a = block[block_h][block_w].q_a;
            int q_b = block[block_h][block_w].q_b;

            for(int bm_h = 0; bm_h < bitmap->bm_h; bm_h++){
                for(int bm_w = 0; bm_w < bitmap->bm_w; bm_w+=2){
                    bm_value += ((int)bitmap->bm[bm_h][bm_w])*2 + (int)bitmap->bm[bm_h][bm_w+1];
                }
            }

            /**取得量化階a,b 的 MSB**/
            int binary_count = this->GetBinaryCount(this->mod);
            bool odd_or_even = binary_count % 2 == 1 ? true : false;

            int lsb_a_count = 0;
            int lsb_b_count = 0;

            if(odd_or_even == true){
                binary_count++;
                lsb_a_count = binary_count/2;
                lsb_b_count = binary_count/2;
            }
            else{
                lsb_a_count = binary_count/2;
                lsb_b_count = binary_count/2;
            }

            int* binary_a_array = new int[8];
            int* binary_b_array = new int[8];

            this->GetBinaryNum(binary_a_array, q_a, 8);
            this->GetBinaryNum(binary_b_array, q_b, 8);


            int msb_a_decimal = GetDecimalNum(binary_a_array, 8 - lsb_a_count, 8);
            int msb_b_decimal = GetDecimalNum(binary_b_array, 8 - lsb_b_count, 8);

            int hash_value = this->HashFunction(bm_value,random,msb_a_decimal,msb_b_decimal);//bm_value*random*(msb_a_decimal + msb_b_decimal);
            int remainder = hash_value % this->mod;

            int* binary_code = new int[binary_count];
            this->GetBinaryNum(binary_code, remainder, binary_count);

            int lsb_a_decimal = q_a - msb_a_decimal;
            int lsb_b_decimal = q_b - msb_b_decimal;

            int* lsb_a_array = new int[lsb_a_count];
            int* lsb_b_array = new int[lsb_b_count];

            this->GetBinaryNum(lsb_a_array, lsb_a_decimal, lsb_a_count);
            this->GetBinaryNum(lsb_b_array, lsb_b_decimal, lsb_b_count);

            for(int i = 0; i < binary_count; i++){
                if(i < lsb_a_count){
                    if(lsb_a_array[i] != binary_code[i]){
                        std::cout << lsb_a_array[0] << " " << lsb_a_array[1] << std::endl;
                        std::cout << binary_code[0] << " " << binary_code[1] << std::endl;
                        this->dirtyCount++;
                        block[block_h][block_w].dirty = true;
                        std::cout << "Error at : " << block_h << " " << block_w << " " << std::endl;
                        break;
                    }
                }
                else{
                    if(lsb_b_array[i - lsb_a_count] != binary_code[i]){
                        this->dirtyCount++;
                        block[block_h][block_w].dirty = true;
                        std::cout << "Error at : " << block_h << " " << block_w << " " << std::endl;
                        break;
                    }
                }
            }
        }
    }

    std::cout << "Roughly Detected : " << this->dirtyCount << std::endl;
}

int BmCombineQuan_Sec::GetDirtyCount(){
    return this->dirtyCount;
}
