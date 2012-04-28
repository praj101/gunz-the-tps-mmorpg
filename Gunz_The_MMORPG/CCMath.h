#pragma once
#include <math.h>
#include <crtdbg.h>

#define PI 3.141592f
#define ToRadian( degree ) ((degree) * (PI / 180.0f))
#define ToDegree( radian ) ((radian) * (180.0f / PI))


///Min is calculated from a random number to max. (NMin, nMax is inclusive of.)
inline int RandomNumber(int nMin, int nMax)
{
	//Range is greater than the size of the overflow is 131072.
	_ASSERT((nMax-nMin) < 131072);
	return (rand() * (nMax - nMin + 1) / (RAND_MAX+1) + nMin );
}

///Min is calculated from a random number to max.
inline float RandomNumber(float fMin, float fMax)
{
	float fRandom = (float)rand() / (float)RAND_MAX;
	return(fRandom*(fMax-fMin)+fMin);
}

///D & d expression is obtained for a random dice. - Sides of the die, in addition mod n dogs throwing out the return value
inline long Dice(unsigned char n, unsigned char sides, short mod)
{
    int result = mod;
    for(int i = 0; i < n; i++)
    {
		result += RandomNumber(1, sides);
    }
    return result;
}

inline float Roundf(float x)
{
	return floorf(x + .5f);
}
