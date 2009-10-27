/*
 * language.h
 *
 *  Created on: Sep 28, 2009
 *      Author: monfee
 */

#ifndef LANGUAGE_H_
#define LANGUAGE_H_

namespace QLINK {

	class language
	{
	public:
		static const char *EN_STOP_WORDS[];
		static int EN_STOP_WORDS_LENGTH;

	public:
		language();
		virtual ~language();

		static bool isstopword(const char *term);
		static int string_compare(const void *a, const void *b);
	};

}

#endif /* LANGUAGE_H_ */
