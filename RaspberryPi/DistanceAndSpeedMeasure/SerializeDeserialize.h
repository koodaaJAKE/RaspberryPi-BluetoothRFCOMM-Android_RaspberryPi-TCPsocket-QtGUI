/*
 * SerializeDeserialize.h
 *
 *  Created on: 31.3.2016
 *      Author: root
 */

#ifndef SERIALIZEDESERIALIZE_H_
#define SERIALIZEDESERIALIZE_H_

#include <stdio.h>
#include "HRLVEZ0.h"

/* Macros for serializing and deserializing 32bit float */
#define Serialize754_32(f) (Serialize754Float((f), 32, 8))
#define Deserialize754_32(i) (Deserialize754Float((i), 32, 8))

/* Function prototypes */
unsigned char *Serialize_Int(unsigned char *buffer, int value);
unsigned char *Serialize_Float(unsigned char *buffer, float FloatValue);
unsigned int Serialize754Float(float f, unsigned int bits, unsigned int expbits);
float Deserialize754Float(unsigned int i, unsigned int bits, unsigned int expbits);
unsigned char *Serialize_Struct(unsigned char *buffer, const HRLVEZ0_Data_t *Data);


#endif /* SERIALIZEDESERIALIZE_H_ */
