#ifndef CHOCOHV_MM_H
#define CHOCOHV_MM_H
#include "util/type.h"
#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE   			(1 << PAGE_SHIFT)
#define SECTION_SIZE			(1 << SECTION_SHIFT)
#define LOW_MEMORY              	(2 * SECTION_SIZE)
#define EL1_LOW_MEMORY              	(2 * 2 * SECTION_SIZE)

#ifndef __ASSEMBLER__
extern void memzero(uint64_t , uint64_t);
#endif

#endif //CHOCOHV_MM_H
