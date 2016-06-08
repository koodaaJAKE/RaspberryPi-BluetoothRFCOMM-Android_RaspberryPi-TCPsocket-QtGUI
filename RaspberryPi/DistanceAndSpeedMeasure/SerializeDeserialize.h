/*
 * SerializeDeserialize.h
 */

#ifndef SERIALIZEDESERIALIZE_H_
#define SERIALIZEDESERIALIZE_H_

#include "thread.h"

/* Macros for serializing and deserializing 32bit float */
#define Serialize754_32(f) (Serialize754Float((f), 32, 8))
#define Deserialize754_32(f) (Deserialize754Float((f), 32, 8))

/* Function prototypes */
unsigned char *Serialize_Int(unsigned char *buffer, int value);
unsigned char *Serialize_Float(unsigned char *buffer, float FloatValue);
unsigned int Serialize754Float(float f, unsigned int bits, unsigned int expbits);
float Deserialize754Float(unsigned int f, unsigned int bits, unsigned int expbits);
unsigned char *Serialize_Struct(unsigned char *buffer, const thread_data_t *Data);


#endif /* SERIALIZEDESERIALIZE_H_ */
