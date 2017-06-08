#ifndef BMCOMBINEQUAN_SEC_H
#define BMCOMBINEQUAN_SEC_H

#include "BTC.h"
#include "MyAlgorithmMethod.h"
#include <cstdlib>
#include <cmath>
#include <functional>
#include <sstream>


class BmCombineQuan_Sec : public MyAlgorithmMethod<Block>
{
    public:
        BmCombineQuan_Sec();
        virtual ~BmCombineQuan_Sec();
        virtual void Embed(Block** block, int randomSeed) override;
        virtual void Detected(Block** block, int randomSeed) override;
        virtual int GetDirtyCount() override;

    protected:

    private:
        const int H = 512;
        const int W = 512;

        const int b_h = 4;
        const int b_w = 4;

        const int mod = 16;

        int dirtyCount = 0;

        int GetBinaryCount(int num);
        void GetBinaryNum(int* num_array, int num, int size);
        int GetDecimalNum(int* num_array, int size);
        int GetDecimalNum(int* num_array, int index, int size);
        int HashFunction(int bm_value, int random, int msb_a, int msb_b);
};

#endif // BMCOMBINEQUAN_SEC_H
