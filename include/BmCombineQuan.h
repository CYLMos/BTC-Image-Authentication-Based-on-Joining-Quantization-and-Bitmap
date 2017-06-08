#ifndef BMCOMBINEQUAN_H
#define BMCOMBINEQUAN_H

//#ifndef BTC_H
//#define BTC_H

#include "BTC.h"

//#endif // BTC_H

#include "MyAlgorithmMethod.h"
#include <cstdlib>


class BmCombineQuan : public MyAlgorithmMethod<Block>
{
    public:
        BmCombineQuan();
        virtual ~BmCombineQuan();
        virtual void Embed(Block** block, int randomSeed) override;
        virtual void Detected(Block** block, int randomSeed) override;
        virtual int GetDirtyCount() override;

        /**attack test**/
        void Attack(Block** block);

    protected:

    private:
        const int H = 512;
        const int W = 512;

        const int b_h = 4;
        const int b_w = 4;

        const int mod = 16;

        int dirtyCount = 0;

        void AdjustQuan(Block** block, int h, int w, int randAuth, int qMod, int bm_value);
};

#endif // BMCOMBINEQUAN_H
