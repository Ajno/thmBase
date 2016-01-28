/*
 * humidity.c
 *
 *  Created on: Dec 14, 2015
 *      Author: z0037ysj
 */

#include "humidity.h"
#include "oscillator.h"

/*
 * f[Hz] vs RH[%] - for 25 deg
 * {f,quotient,offset,scale_1,scale_2}
 * RH=(quotient*f/scale_1+offset)/scale_2
 */
static Word freqVsHum[10][5] =
{
{ 21, 0, 20, 1, 1 }, //quot=0, off=20, scale_1=1, scale_2=1
{ 84, 650, 8533, 8, 512 }, //quot=650=0.1587302*4096, off=8533=16.66667*512, scale_1=8=4096/512, scale_2=512
{ 300, 192, 6684, 16, 256 }, //quot=190=0.0462963*4096, off=6684=256*26.11111111, scale_1=16, scale_2=256
{ 1050, 55, 4608, 32, 128 }, //quot=55=4096*0.01333333333, off=4608=36*128, scale_1=32, scale_2=128
{ 1750, 29, 10880, 16, 256 }, //quot=29=0.00714285714*4096, off=10880=256*42.500000005, scale_1=16, scale_2=256
{ 3232, 5/(3232-1750)*16*256*3232, (60-5/(3232-1750)*3232)*256, 16, 256 }, //60
{ 9549, 10/(9549-3232)*9549*2048, (70-10/(9549-3232)*9549)*512, 4, 512 }, //70
{ 21008, 10/(21008-9549)*21008*2048, (80-10/(21008-9549)*21008)*256, 8, 256 }, //80
{ 45670, 0, 90, 1, 1 }, //quot=2392=65536*0.00040548211, off=71.4816320363
{ 61789, 0, 95, 1, 1 } }; //quot=1=0.00031019294*3223, off=75.8334884303, scale_1=3223, scale_2=1

/*
 * t[10xdeg] vs RH[%] error relative to reference 25 deg
 */
static sWord tempVsHumErr[3][2] =
{
{ 50, 10 },
{ 150, 5 },
{ 300, -2 } };

static Word frequency2humidity(const Word frequency)
{
    Word ret = 0;
    Byte idx = 0;

    if (freqVsHum[3][idx] >= frequency)
    {
        if (freqVsHum[1][idx] >= frequency)
        {
            if (freqVsHum[0][idx] >= frequency)
            {
                idx = 0;
            }
            else
            {
                idx = 1;
            }
        }
        else
        {
            if (freqVsHum[2][idx] >= frequency)
            {
                idx = 2;
            }
            else
            {
                idx = 3;
            }
        }
    }
    else
    {
        if (freqVsHum[6][idx] >= frequency)
        {
            if (freqVsHum[5][idx] >= frequency)
            {
                if (freqVsHum[4][idx] >= frequency)
                {
                    idx = 4;
                }
                else
                {
                    idx = 5;
                }
            }
            else
            {
                idx = 6;
            }
        }
        else
        {
            if (freqVsHum[7][idx] >= frequency)
            {
                idx = 7;
            }
            else
            {
                if (freqVsHum[8][idx] >= frequency)
                {
                    idx = 8;
                }
                else
                {
                    idx = 9;
                }
            }
        }
    }

    // RH=(frequency*quotient/scale_1+offset)/scale_2
    return (frequency * freqVsHum[idx][1] / freqVsHum[idx][3]
            + freqVsHum[idx][2]) / freqVsHum[idx][4];
}

static void compensate4Temperature(const sWord cTemperature, Word* pHumidity)
{
    if (250 > cTemperature)
    {
        if (tempVsHumErr[0][0] > cTemperature)
        {
            *pHumidity += tempVsHumErr[0][1];
        }
        else if (tempVsHumErr[1][0] > cTemperature)
        {
            *pHumidity += tempVsHumErr[1][1];
        }
    }
    else
    {
        if (tempVsHumErr[2][0] < cTemperature)
        {
            *pHumidity += tempVsHumErr[2][1];
        }
    }
}

Word humidityRead(const sWord cTemperature, const Word cSamplingFrequency)
{
    Word humidity = oscillatorReadAndClear();
    humidity = frequency2humidity(humidity * cSamplingFrequency);
    compensate4Temperature(cTemperature, &humidity);
    return humidity;
}
