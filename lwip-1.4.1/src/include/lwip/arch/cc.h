#ifndef __CC_H__
#define __CC_H__

#include "../arch.h"

#if 0
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN  3412
#endif /* LITTLE_ENDIAN */

#ifndef BIG_ENDIAN
#define BIG_ENDIAN     1234
#endif /* BIGE_ENDIAN */
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER     LITTLE_ENDIAN
#endif /* BYTE_ORDER */


typedef unsigned   char    u8_t;    /* Unsigned 8 bit quantity         */
typedef signed     char    s8_t;    /* Signed    8 bit quantity        */
typedef unsigned   short   u16_t;   /* Unsigned 16 bit quantity        */
typedef signed     short   s16_t;   /* Signed   16 bit quantity        */
typedef unsigned   int    u32_t;   /* Unsigned 32 bit quantity        */
typedef signed     int    s32_t;   /* Signed   32 bit quantity        */
typedef u32_t mem_ptr_t;            /* Unsigned 32 bit quantity        */
typedef int sys_prot_t;




/*----------------------------------------------------------------------------*/
/* define compiler specific symbols */
#if defined (__ICCARM__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT 
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES

#elif defined (__CC_ARM)
#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT 
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__GNUC__)
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__TASKING__)
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif


/*---define (sn)printf formatters for these lwip types, for lwip DEBUG/STATS--*/
#define U16_F	"d"
#define S16_F	"d"
#define X16_F	"x"
#define U32_F	"ld"
#define S32_F	"ld"
#define X32_F	"lx"




#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) \
	do { \
		printf("\x1b[32m Assertion \"%s\" failed at line %d in %s\x1b[0m\r\n", x, __LINE__, __FILE__); \
	} while(0)
#endif

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)
/*
#define LWIP_PLATFORM_DIAG(fmt, ...) \
do { \
	printf(fmt, ##  __VA_ARGS__); \
} while (0)
*/
#endif
#endif /* __CC_H__ */
