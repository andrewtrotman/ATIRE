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

#ifndef STPL_STREAM_H_
#define STPL_STREAM_H_

/*
 * modified from the bewlow source:
 * http://www.cplusplus.com/doc/tutorial/files.html
 */

#include <iostream>
#include <fstream>
#include <climits>
#include <cstring>

#include "stpl_doc.h"

namespace stpl {
	/**
	 *
	 */
	template<
			typename StringT = std::string,
			typename IteratorT = char */*typename StringT::iterator*/,
			typename DocumentT = Document<StringBound<StringT, IteratorT> >
			>
	class FileStream : public DocumentT {
		private:
			enum mode { MEMORY, FILE, MEMORY_FILE };

		public:
			typedef typename DocumentT::entity_type				entity_type;
			typedef typename entity_type::string_type			string_type;
			typedef typename entity_type::iterator				iterator;
			static const unsigned long long						BUFFER_SIZE = 20971520;

		private:
			ifstream::pos_type 				size_;
			ifstream::pos_type 				count_;
			char 							*memblock_;
			ifstream 						file_;
			string							filename_;
			mode							mode_;
			unsigned long long				buf_size_;

		private:
			void cleanup();
			void init();

		public:
			FileStream(string filename, mode read_mode = MEMORY, unsigned long long buf_size = BUFFER_SIZE):
				DocumentT::Document(), memblock_(0), filename_(filename), mode_(read_mode), buf_size_(BUFFER_SIZE) {
				init();
				if (mode_ == MEMORY)
					read();
			}

			FileStream(mode read_mode = MEMORY, unsigned long long buf_size = BUFFER_SIZE):
				DocumentT::Document(), memblock_(0), buf_size_(BUFFER_SIZE)
			{
				init();
			}

			~FileStream() {
				cleanup();
			}

			inline void read() {
				read(filename_);
			}

			void read(string filename);

			bool next_buffer();

			/**
			 * return the beginning and the end of the buffer
			 */
			inline char* begin() { return memblock_; }
			inline char* end() {
				return memblock_ + strlen(memblock_);
			}

			inline bool is_end() {
				if (count_ < size_)
					return false;
				return true;
			}
	};

}

#include "stpl_stream.tcc"

#endif /*STPL_STREAM_H_*/
