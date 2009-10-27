/******************************************************************************
 * This file is part of the Simple Text Processing Library(STPL).
 * (c) Copyright 2007-2008 TYONLINE TECHNOLOGY PTY. LTD.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU LESSER GENERAL PUBLIC LICENSE, Version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *******************************************************************************
 *
 * @author				Ling-Xiang(Eric) Tang
 *
 *******************************************************************************/


#ifndef STPL_UNICODE_H_
#define STPL_UNICODE_H_

#include "stpl_character.h"
#include <cmath>
/* for debug
 */
//#include <iostream>

namespace stpl {

	namespace UNICODE {
		/**
		 * The number of maximum bytes is six if in utf8 encoding:
		 * 	b1 - C0–DF, E0-EF, F0-F4, F8-FB, FC-FD
		 *  b2 - 80–BF
		 *  b3 - 80–BF
		 *  b4 - 80–BF
		 *  b5 - 80–BF
		 *  b6 - 80–BF
		 */
		template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
		class Utf8 {
			public:
				static bool is_valid_char(IteratorT it, int& num_of_bytes) {
					num_of_bytes = 1;
					//if (Characters<StringT, IteratorT>::is_valid_char(it))
					unsigned char code = *it;
					//std::cout << "code: " << static_cast<int>(code) << std::endl;

					if ((0 <= code && code <= 0x7F))
						return true;

					if (code >= 0xFC && code <=0xFD)
						num_of_bytes = 6;
					else if (code >= 0xF8 && code <= 0xFB)
						num_of_bytes = 5;
					else if (code >= 0xF0 && code <= 0xF4)
						num_of_bytes = 4;
					else if (code >= 0xE0 && code <= 0xEF)
						num_of_bytes = 3;
					else if (code >= 0xC0 && code <= 0xDF)
						num_of_bytes = 2;
					else
						return false; // it doesn't fall any utf-8 first byte range

					for (int i = 0; i< num_of_bytes - 1; i++) {
						++it;
						code = *it;
						if (code < 0x80 || code > 0xBF)
							return false;
					}
					return true;
				}

				static int is_valid_char(IteratorT it) {
					int num_of_bytes;
					return is_valid_char(it, num_of_bytes) ? num_of_bytes : 0;
				}

				/**
				 * @param character iterator
				 * @return number of bytes
				 *
				 * if the given char is not a valid utf8 encoding char
				 * it will be considered as a 1-byte char anyway
				 */
				static int bytes(IteratorT it) {
					int num_of_bytes = 1;
					return is_valid_char(it, num_of_bytes) ? num_of_bytes : 1;
				}

				static IteratorT utf8_char(IteratorT it) {
					return it + bytes(it);
				}

				static StringT first_utf8_char(IteratorT it) {
					return StringT(it, utf8_char(it));
				}

				static StringT first_utf8_char(StringT& source) {
					return first_utf8_character(source.begin());
				}

				static int size(StringT& source) {
					int count = 0;
					IteratorT it = source.begin();
					for (; it != source.end(); ) {
						it = utf8_char(it);
						count++;
					}
					return count;
				}

				static int length(StringT& source) {
					return size(source);
				}

				/**
				 * 	@return the unicode code point
				 * 	@param - the input has to be a valid utf8 character
				 *
				 * 	maximun number of bytes are six
				 */
				static unsigned int code(StringT& utf8_char) {
					const unsigned int br_c = 6;  // maximun bytes of utf8 encoding
					const unsigned int min_c = 128;
					const unsigned int max_c = 1;
					const unsigned int max_rest = 63;
					unsigned int low_max = max_c;
					unsigned int high_min = min_c;
					unsigned int code = 0;

					int length = utf8_char.end() - utf8_char.begin();
					if (length <= 6 && length > 0) {
						unsigned char b1 = utf8_char[0];

						if (utf8_char.length() == 1)
							return b1;

						// deciding the first byte valid or not
						unsigned int i;
						for (i = 0; i < (utf8_char.length() - 1); i++) {
							high_min = (high_min >> 1) + min_c;
						}

						for (i = 0; i < (7 - (utf8_char.length() + 1)); i++) {
							low_max = (low_max << 1) + max_c;
						}

						if (b1 <= (high_min + low_max) && b1 >= high_min) {
							code = b1 - high_min;

							for (i = 1; i < utf8_char.length(); i++) {

								unsigned char b = utf8_char[i];
								if (b < min_c || b > (min_c + max_rest)) {
									code = -2;
									break;
								}

								code = code << br_c;
								code += (b - min_c);
							}

						}
					}
					// TODO if code = -1, throw a exception say this is not a valid utf8 char
					return code;
				}
		};
	}

}

#endif /*STPL_UNICODE_H_*/
