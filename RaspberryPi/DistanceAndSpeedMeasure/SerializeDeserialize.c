/*
 * SerializeDeserialize.c
 *
 *  Created on: 31.3.2016
 *      Author: root
 */
#include "SerializeDeserialize.h"

unsigned char *Serialize_Int(unsigned char *buffer, int value)
{
  /* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;

  return buffer + 4;
}

unsigned char *Serialize_Float(unsigned char *buffer, float FloatValue)
{
	int value;

	value = Serialize754_32(FloatValue);

    buffer[0] = value >> 24;
    buffer[1] = value >> 16;
    buffer[2] = value >> 8;
    buffer[3] = value;

    return buffer + 4;
}

unsigned int Serialize754Float(float f, unsigned int bits, unsigned int expbits)
{
    float fnorm;
    int shift;
    int sign, exp, significand;
    unsigned int significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0)
    {
    	sign = 1;
    	fnorm = -f;
    }
    else
    {
    	sign = 0;
    	fnorm = f;
    }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0)
    {
    	fnorm /= 2.0;
    	shift++;
    }
    while(fnorm < 1.0)
    {
    	fnorm *= 2.0;
    	shift--;
    }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significant data
    significand = fnorm * ((1<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

float Deserialize754Float(unsigned int i, unsigned int bits, unsigned int expbits)
{
    float result;
    int shift;
    unsigned int bias;
    unsigned int significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significant
    result = (i&((1<<significandbits)-1)); // mask
    result /= (1<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

unsigned char *Serialize_Struct(unsigned char *buffer, const HRLVEZ0_Data_t *Data)
{
	buffer = Serialize_Int(buffer, Data->distance);
	buffer = Serialize_Float(buffer, Data->speed);
	buffer = Serialize_Int(buffer, Data->prevdistance);
	return buffer;
}

