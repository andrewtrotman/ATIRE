/*
	MERSENNE_TWISTER.H
	------------------
	Standard reference implementation of the Mersenne Twister random number generator
   The original was coded by Takuji Nishimura and Makoto Matsumoto.

	Converted into a C++ class by Andrew Trotman
*/
#ifndef MERSENNE_TWISTER_H_
#define MERSENNE_TWISTER_H_

/*
	class ANT_MERSENNE_TWISTER
	--------------------------
*/
class ANT_mersenne_twister
{
private:
	static const size_t NN = 312;
	static const size_t MM = 156;
	static const unsigned long long MATRIX_A = 0xB5026F5AA96619E9ULL;
	static const unsigned long long UM = 0xFFFFFFFF80000000ULL;		/* Most significant 33 bits */
	static const unsigned long long LM = 0x7FFFFFFFULL;				/* Least significant 31 bits */

private:
	/*
		The array for the state vector
	*/
	unsigned long long mt[NN]; 
	int mti;

public:
	ANT_mersenne_twister() { mti = NN + 1; }		/* mti==NN+1 means mt[NN] is not initialized */
	/*
		initializes mt[NN] with a seed
	*/
	void init_genrand64(unsigned long long seed);

	/*
		initialize by an array with array-length init_key is the array for initializing keys key_length is its length
	*/
	void init_by_array64(unsigned long long init_key[], unsigned long long key_length);

	/*
		generates a random number on [0, 2^64-1]-interval
	*/
	unsigned long long genrand64_int64(void);

	/*
		generates a random number on [0, 2^63-1]-interval
	*/
	inline long long genrand64_int63(void) { return (long long)(genrand64_int64() >> 1); }

	/*
		generates a random number on [0,1]-real-interval
	*/
	inline double genrand64_real1(void) { return (genrand64_int64() >> 11) * (1.0/9007199254740991.0); }

	/*
		generates a random number on [0,1)-real-interval
	*/
	inline double genrand64_real2(void) { return (genrand64_int64() >> 11) * (1.0/9007199254740992.0); }

	/*
		generates a random number on (0,1)-real-interval
	*/
	inline double genrand64_real3(void) { return ((genrand64_int64() >> 12) + 0.5) * (1.0/4503599627370496.0); }
} ;

#endif /* MERSENNE_TWISTER_H_ */
