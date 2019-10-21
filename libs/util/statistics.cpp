#include "statistics.h"
#include "math.h"

StatisticBuffer::StatisticBuffer(int _nMaxSize, bool _calcMeanImmediately, bool _calcStdDevImmediately)
{
    pBuffer = new float[_nMaxSize];
    nMaxSize = _nMaxSize;
    nSize = _nMaxSize;
    calcMeanImmediately = _calcMeanImmediately;
    calcStdDevImmediately = _calcStdDevImmediately;
    clear();
}

void StatisticBuffer::clear()
{
    nValues = 0;
    nWriteIndex = 0;
    minValue = FLT_MAX;
    maxValue = FLT_MIN;
    range = 0.0f;
    stdDev = 0.0f;
    mean = 0.0f;
}


/*
 *  runtime costs @LPC1347 72MHz: 25 µs for  8 values mean, 68 µs for mean/stddev
 *                                32 µs for 10 values mean, 80 µs for mean/stddev
 *                                143 µs for 20 values mean/stddev
*/

int StatisticBuffer::add(float Value)
{
    // update min/max
    if (Value < minValue)
        minValue = Value;
    if (Value > maxValue)
        maxValue = Value;
    range = maxValue - minValue;

    if (nValues < nSize)
        nValues++;

    pBuffer[nWriteIndex] = Value;
    nWriteIndex < (nSize-1) ? nWriteIndex++ : nWriteIndex = 0;
    
    if (calcMeanImmediately || calcStdDevImmediately)
    {
        //double sum = 0.0;
        float sum = 0.0;
        int i;
    
        sum = 0.0;
        for (i = 0; i < nValues; i++)
            sum += pBuffer[i];
        mean = sum / i;
    }

    if (calcStdDevImmediately)
    {
        //double sum = 0.0;
        float sum = 0.0;
        int i;
        
        for (i = 0; i < nValues; i++)
            sum += ((pBuffer[i] - mean) * (pBuffer[i] - mean));
        stdDev = sqrt(sum / (nValues - 1));
    }
    
    return nValues;
}

/*
 *  update internal results when calcImmediately is false
 *  
*/

void StatisticBuffer::calcStatistics()
{
    //double sum = 0.0;
    float sum = 0.0;
    int i;

    sum = 0.0;
    for (i = 0; i < nValues; i++)
        sum += pBuffer[i];
    mean = sum / i;

    sum = 0.0;
    for (i = 0; i < nValues; i++)
        sum += ((pBuffer[i] - mean) * (pBuffer[i] - mean));
    stdDev = sqrt(sum / (nValues - 1));
}

   
void StatisticBuffer::resetMinMax()
{
    minValue = maxValue = 0.0;
}


