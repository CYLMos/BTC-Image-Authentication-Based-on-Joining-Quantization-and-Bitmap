#ifndef MYALGORITHMMETHOD_H
#define MYALGORITHMMETHOD_H



template <class T>
class MyAlgorithmMethod
{
    public:
        virtual ~MyAlgorithmMethod(){};
        virtual void Embed(T** block, int randomSeed) = 0;
        virtual void Detected(T** block, int randomSeed) = 0;
        virtual int GetDirtyCount() = 0;
};

#endif // MYALGORITHMMETHOD_H
