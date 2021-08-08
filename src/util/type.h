#ifndef KOOPAHV_TYPE_H
#define KOOPAHV_TYPE_H

#ifndef __ASSEMBLER__
#define bool	_Bool
#define true	1
#define false	0

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __UINTPTR_TYPE__ uintptr_t;

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char uint8_t;
typedef unsigned long long size_t;
typedef long int                intmax_t;
typedef unsigned long int       uintmax_t;

#endif

#endif //KOOPAHV_TYPE_H
