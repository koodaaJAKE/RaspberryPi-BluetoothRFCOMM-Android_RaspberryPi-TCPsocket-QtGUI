/*
 * SerializeDeserialize.h
 */

#ifndef SERIALIZEDESERIALIZE_H_
#define SERIALIZEDESERIALIZE_H_

#include <stdio.h>
#include "HRLVEZ0.h"

/* Macros for serializing 32bit float */
#define Serialize754_32(f) (Serialize754Float((f), 32, 8))

/* Function prototypes */
unsigned char *Serialize_Int(unsigned char *buffer, int value);
unsigned char *Serialize_Float(unsigned char *buffer, float FloatValue);
unsigned int Serialize754Float(float f, unsigned int bits, unsigned int expbits);
unsigned char *Serialize_Struct(unsigned char *buffer, const HRLVEZ0_Data_t *Data);


#endif /* SERIALIZEDESERIALIZE_H_ */
