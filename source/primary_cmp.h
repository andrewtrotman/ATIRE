
#ifndef PRIMARY_CMP_H
#define PRIMARY_CMP_H

template <typename T> struct Primary_cmp {
	int operator() (const T &a, const T &b) {
		return a - b;
	}
};


#endif
