#ifndef __COUNTER_H__
#define __COUNTER_H__
#ifdef USE_COUNTER
#define INC_COUNTER inc_counter()
#define DEC_COUNTER dec_counter()
#define PRINT_COUNTER print_counter()
extern void inc_counter();
extern void dec_counter();
extern void print_counter();
#else
#define INC_COUNTER 
#define DEC_COUNTER 
#define PRINT_COUNTER 
#endif

#endif
