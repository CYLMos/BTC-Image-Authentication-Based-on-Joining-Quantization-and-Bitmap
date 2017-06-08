#ifndef MYALGORITHM_H
#define MYALGORITHM_H

#include "BTC.h"
#include "MyAlgorithmMethod.h"


class MyAlgorithm
{
    public:
        MyAlgorithm();
        ~MyAlgorithm();

        void SetMethod(MyAlgorithmMethod<Block>* method);
        void SetBlockData(Block** block);
        void StartEmbed();
        void StartDetected();
        void RefinementDetected();
        void OutPutDetected(char* file);
        int GetTotalDirtyCount();

        /** attack method **/
        void AttackImage(unsigned char* imageData, unsigned char* attackData);

    protected:

    private:
        Block** block;
        MyAlgorithmMethod<Block>* method;

        int randomSeed;
        int totalDirtyCount;
};

#endif // MYALGORITHM_H
