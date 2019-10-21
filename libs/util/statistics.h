#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "float.h"

class StatisticBuffer
{
public:
    StatisticBuffer(int nMaxSize, bool calcMeanImmediately = true, bool calcStdDevImmediately = true);
    void clear();
    int calcStatistics(float *pMean, float *pStdDev=0, float *pRange=0);
    void calcStatistics();
    void resetMinMax();
    int add(float Value);
    float getValue(int index) { return pBuffer[index]; };

    // these results are updated after add(Value):    
    float minValue;
    float maxValue;
    float range;
    // these results are only updated when calcImmediate=true
    float mean;
    float stdDev;

private:
    float *pBuffer;
    int nMaxSize;
    int nSize;
    int nValues;
    int nWriteIndex;
    bool calcMeanImmediately;
    bool calcStdDevImmediately;
};

#endif
