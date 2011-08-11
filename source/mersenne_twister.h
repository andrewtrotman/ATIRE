/*
	MERSENNE_TWISTER.H
	------------------
	Standard reference implementation of the Mersenne Twister random number generator
   Coded by Takuji Nishimura and Makoto Matsumoto.
*/
#ifndef MERSENNE_TWISTER_H_
#define MERSENNE_TWISTER_H_

/* initializes mt[NN] with a seed */
void init_genrand64(unsigned long long seed);

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(unsigned long long init_key[], unsigned long long key_length);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void);

/* generates a random number on [0, 2^63-1]-interval */
static inline long long genrand64_int63(void) { return (long long)(genrand64_int64() >> 1); }

/* generates a random number on [0,1]-real-interval */
static inline double genrand64_real1(void) { return (genrand64_int64() >> 11) * (1.0/9007199254740991.0); }

/* generates a random number on [0,1)-real-interval */
static inline double genrand64_real2(void) { return (genrand64_int64() >> 11) * (1.0/9007199254740992.0); }

/* generates a random number on (0,1)-real-interval */
static inline double genrand64_real3(void) { return ((genrand64_int64() >> 12) + 0.5) * (1.0/4503599627370496.0); }

#endif /* MERSENNE_TWISTER_H_ */
