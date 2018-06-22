#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define internal 		static
#define external 		extern
#define null 			((void*)0)
#define nul 			""
#define nil 			'\0'
#define false			(0)
#define true			(!false)

typedef char 			str;
typedef char 			byte;
typedef unsigned char 		ubyte;

typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long int	ulong;

typedef long long int		llong;
typedef unsigned long long int	ullong;

typedef signed char		s8;
typedef signed short		s16;
typedef signed int		s32;
typedef signed long int		s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long int	u64;

#endif // TYPES_H_INCLUDED
