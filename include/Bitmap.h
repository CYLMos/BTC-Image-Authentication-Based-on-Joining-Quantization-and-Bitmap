#include <iostream>

class Bitmap{
    public:
        Bitmap(int h,int w);
        ~Bitmap();
        void ac_generation(int len);
        void ac_embed(int len);
        void ac_embed_improve(int len,int random_classfication[]);
        bool bm_detected_improve(int len,int random_classfication[]);
        int bm_h,bm_w;
        bool** bm = NULL;
        bool* auth_bm = NULL;
        int random_num;
        //****sub_bitmap****
        int sub_bm_h,sub_bm_w;
        bool *parity_value = NULL;

        int change_count = 0;
        int getChangeCount();
    private:
        int get_different_bool_count(int index,int sub_bm_num);
};
