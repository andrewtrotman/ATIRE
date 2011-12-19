/*
	PRIMARY_CMP.H
	-------------
*/
#ifndef PRIMARY_CMP_H_
#define PRIMARY_CMP_H_

/*
	class ANT_PRIMARY_CMP
	---------------------
*/
template <typename T> class ANT_primary_cmp
{
public:
	int operator()(const T &a, const T &b) { return a - b; }
};

#endif /* PRIMARY_CMP_H_ */
