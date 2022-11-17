/*
 * platform.h
 *
 * Created by Ruibin.Chow on 2022/01/25.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

typedef unsigned char Boolean; /* Boolean value type. */
typedef unsigned long int UINT32; /* Unsigned 32 bit value */
typedef unsigned short UINT16; /* Unsigned 16 bit value */
typedef unsigned char UINT8; /* Unsigned 8 bit value */
typedef signed long int INT32; /* Signed 32 bit value */
typedef signed short INT16; /* Signed 16 bit value */
typedef signed char INT8; /* Signed 8 bit value */

//得到指定地址上的一个字节或字
#define MEM_B( x ) ( *( (byte *) (x) ) )
#define MEM_W( x ) ( *( (word *) (x) ) )

//求最大值和最小值
#define MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )

//得到一个field在结构体(struct)中的偏移量
#define FPOS( type, field ) \
    /*lint -e545 */ ( (dword) &(( type *) 0)-> field ) /*lint +e545 */

//得到一个结构体中field所占用的字节数
#define FSIZ( type, field ) sizeof( ((type *) 0)->field )

//按照LSB格式把两个字节转化为一个Word
#define FLIPW( ray ) ( (((word) (ray)[0]) * 256) + (ray)[1] )

//按照LSB格式把一个Word转化为两个字节
#define FLOPW( ray, val ) \
    (ray)[0] = ((val) / 256); \
    (ray)[1] = ((val) & 0xFF)

//得到一个变量的地址(word宽度)
#define B_PTR( var ) ( (byte *) (void *) &(var) )
#define W_PTR( var ) ( (word *) (void *) &(var) )

//得到一个字的高位和低位字节
#define WORD_LO(xxx) ((byte) ((word)(xxx) & 255))
#define WORD_HI(xxx) ((byte) ((word)(xxx) >> 8))

//返回一个比X大的最接近的8的倍数
#define RND8( x ) ((((x) + 7) / 8 ) * 8 )

//将一个字母转换为大写
#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )

//判断字符是不是10进制的数字
#define DECCHK( c ) ((c) >= '0' && (c) <= '9')

//判断字符是不是16进制的数字
#define HEXCHK( c ) ( ((c) >= '0' && (c) <= '9') || \
    ((c) >= 'A' && (c) <= 'F') || \
    ((c) >= 'a' && (c) <= 'f') )

//防止溢出的一个方法
#define INC_SAT( val ) (val = ((val)+1 > (val)) ? (val)+1 : (val))

//返回数组元素的个数
#define ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( (a[0]) ) )

//返回一个无符号数n尾的值MOD_BY_POWER_OF_TWO(X,n)=X%(2^n)
#define MOD_BY_POWER_OF_TWO( val, mod_by ) \
    ( (dword)(val) & (dword)((mod_by)-1) )

//对于IO空间映射在存储空间的结构，输入输出处理
#define inp(port) (*((volatile byte *) (port)))
#define inpw(port) (*((volatile word *) (port)))
#define inpdw(port) (*((volatile dword *)(port)))
#define outp(port, val) (*((volatile byte *) (port)) = ((byte) (val)))
#define outpw(port, val) (*((volatile word *) (port)) = ((word) (val)))
#define outpdw(port, val) (*((volatile dword *) (port)) = ((dword) (val)))


namespace platform {

int thread_set_name(const char*);
const char* thread_get_current_name();

UINT16 thread_get_current_id(void);


}

#endif /* !PLATFORM_H */
