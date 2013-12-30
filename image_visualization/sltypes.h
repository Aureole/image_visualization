#ifndef IMAGE_VISUALIZATION_TYPES_H_ 
#define IMAGE_VISUALIZATION_TYPES_H_ 

typedef unsigned char  sl_uint8;
typedef unsigned short sl_uint16;
typedef unsigned  int  sl_uint32;
typedef unsigned long  sl_uint64;

typedef    char  sl_int8;  
typedef __int16  sl_int16;
typedef __int32  sl_int32;
typedef __int64  sl_int64;

#define sl_null  0

typedef __int32  sl_bool;
#define sl_true  (sl_bool)1
#define sl_false (sl_bool)0

typedef struct SlBound
{
	sl_int32 left;
	sl_int32 right;
	sl_int32 top;
	sl_int32 bottom;
}SlBound;

#endif