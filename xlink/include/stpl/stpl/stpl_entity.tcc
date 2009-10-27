/******************************************************************************
 * This file is part of the STPL.
 * (c) Copyright 2007-2009 TYONLINE TECHNOLOGY PTY LTD
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

#include <string>
#include "../utils/icstring.h"

//using namespace stpl;
namespace stpl {
/*
	template <>
	StringBound<std::string, char*>::StringBound() :
		 begin_((char*)content_.c_str()), end_(begin_ + content_.length()) {
	}

	template <>
	StringBound<icstring, char*>::StringBound() :
		 begin_((char*)content_.c_str()), end_(begin_ + content_.length()) {
	}

	template <>
	StringBound<std::string, char*>::StringBound(std::string content) :
		 begin_((char*)content.c_str()), end_(begin_ + content.length()) {
	}

	template <>
	StringBound<icstring, char*>::StringBound(icstring content) :
		 begin_((char*)content.c_str()), end_(begin_ + content.length()) {
	}


	template<>
	void StringBound<std::string, char*>::ref(std::string& ref) {
		//content_ref_ = ref;
		begin((char*)content_ref_.c_str());
		end(begin() + content_ref_.length());
	}

	template<>
	void StringBound<icstring, char*>::ref(icstring& ref) {
		//content_ref_ = ref;
		begin((char*)content_ref_.c_str());
		end(begin() + content_ref_.length());
	}

	template<>
	void StringBound<std::string, char*>::set_end() {
		this->end((char*)content_ref_.c_str() + content_ref_.length());
	}

	template<>
	void StringBound<icstring, char*>::set_end() {
		this->end((char*)content_ref_.c_str() + content_ref_.length());
	}
	*/
}

/*
 *
template <typename EntityT, typename ContainerT>
bool Entity<EntityT, ContainerT>::more() {
	if (children_.size() > 0) {
		//if (!current_pos_)
		//	reset();

		if (current_pos_ != children_.end())
			return true;
	}
	return false;
}
*/
