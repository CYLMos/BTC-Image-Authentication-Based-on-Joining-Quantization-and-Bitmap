#ifndef QUANCOMBINEBM_H
#define QUANCOMBINEBM_H

#include "BTC.h"

#include "MyAlgorithmMethod.h"
#include <cstdlib>
#include <cmath>

class QuanCombineBm : public MyAlgorithmMethod<Block>
{
    public:
        QuanCombineBm();
        QuanCombineBm(int len,int seed);
        virtual ~QuanCombineBm();
        virtual void Embed(Block** block, int randomSeed) override;
        virtual void Detected(Block** block, int randomSeed) override;
        virtual int GetDirtyCount() override;

        void SetLenValue(int len);
        void SetBmSeedValue(int seed);

    protected:

    private:
        void CodeGeneration(Block** block, int randomSeed);

        /** Embed **/
        void BmDivideAndEnbed(Block** block);
        void EmbedInBm(Bitmap* bm, int* classfication);
        int GetDifferentBoolCount(int index, int sub_bm_num, bool** bm);

        const int H = 512;
        const int W = 512;

        const int b_h = 4;
        const int b_w = 4;

        int len = 0;
        int bm_seed = 0;

        int dirtyCount = 0;
};

#endif // QUANCOMBINEBM_H
