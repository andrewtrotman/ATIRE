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

#ifndef STPL_CHARACTERS_H_
#define STPL_CHARACTERS_H_

#include <string>

namespace stpl {

	enum Language {
		UNASSIGNED = 0,
		UNKNOWN = 1,
		NUMBER = 2 << 0,
		ENGLISH = 2 << 1,
		CHINESE = 2 << 2,
		PUNCTUATION = 2 << 3,
		SYMBOL = 2 << 4,
		SPACE = 2 << 5
	};

	template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
	class Character {
		public:
			//virtual bool is_valid_char(IteratorT it) = 0;
			virtual Language lang() { return UNKNOWN; }
	};

	// The Characters class defined here are the ASCII characters
	template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
	class ASCII {
		public:
			static bool is_valid_char(IteratorT it) {
				int t = (int)(*it);
				return (0 <= t && t <= 0x7F);
			}

	};
}

#endif /*STPL_CHARACTERS_H_*/
