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

#ifndef STPL_CHINESE_H_
#define STPL_CHINESE_H_

#include "stpl_unicode.h"

namespace stpl {
	namespace UNICODE {
		/*
		 * Chinese Unicode code point range from 0x4e00 to 0x9fa5
		 *
		 * Unicode 4.10: Change in the end of the CJK Unified Ideographs range from U+9FA5 to U+9FBB
		 */
		template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
		class Chinese : public Utf8<StringT, IteratorT> {
			public:
				static bool is_valid_char(StringT unicode_char) {
					//int length = unicode_char.end() - unicode_char.begin();
					//if (length > 0)
					if (unicode_char.length() > 0)
						return is_valid_char(Utf8<StringT, IteratorT>::code(unicode_char));
					return false;
				}

				static bool is_valid_char(unsigned int code) {
					return ((code >= 0x4e00 && code <= 0x9fff)
						|| (code >= 0x3400 && code <= 0x4dbf)
						|| (code >=0x20000 && code <= 0x2a6df)
						|| (code >=0xf900 && code <= 0xfaff)
						|| (code >=0x2f800 && code <= 0x2fa1f) );
				}

				static bool is_valid_char(IteratorT it) {
					return is_valid_char(first_utf8_char(it));
				}
		};
	}
}
#endif /*STPL_CHINESE_H_*/
