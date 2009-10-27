/*
 * str.h
 *
 *  Created on: Oct 6, 2009
 *      Author: monfee
 */

#ifndef STR_H_
#define STR_H_

/*
 * This is evil code
 */
inline const char *strbck(const char *source, const char* start)
{
	while (start != source && !isspace(*start))
		--start;

	if (isspace(*start))
		start++;
	return start;
}

/*
 * This is evil code
 */
inline const char *strfwd(const char *end, const char* start)
{
	while (start < end && !isspace(*start))
		++start;

//	if (isspace(*start))
//		start--;
	return start;
}

#endif /* STR_H_ */
