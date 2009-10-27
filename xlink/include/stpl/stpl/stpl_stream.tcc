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

#include <stdio.h>
#include <stdexcept>

namespace stpl
{
	using namespace std;
	template<typename DocumentT>
	void FileStream<DocumentT>::cleanup() {
		if (memblock_)
			delete[] memblock_;
		if (file_.is_open())
			file_.close();
		size_ = count_ = 0;
	}

	template<typename DocumentT>
	void FileStream<DocumentT>::init() {
	}

	template<typename DocumentT>
	void FileStream<DocumentT>::read(string filename)
	{
		file_.open(filename.c_str(),  ios::in|ios::binary|ios::ate);
		count_ = 0;

		if (file_.is_open())
		{
		  size_ = file_.tellg();
		  file_.seekg (0, ios::beg);
		  if (size_ > buf_size_) {
			memblock_ = new char [static_cast<int>(buf_size_) + 1];
			file_.read (memblock_, buf_size_);
			count_ += buf_size_;
		  }
		  else {
			memblock_ = new char [static_cast<int>(size_) + 1];
			file_.read (memblock_, size_);
			count_ = size_;
		  }
		  memblock_[count_] = '\0';
		  //cout << "the complete file content is in memory";

		}
		/**
		 * TODO throw an exception when can't open the file
		 */
		else  throw std::runtime_error(filename + ": Unable to open file"); //cout << "Unable to open file" << endl;
	}

	template<typename DocumentT>
	bool FileStream<DocumentT>::next_buffer() {
	    if (!file_.eof() && (size_ > buf_size_)) {
	    	if ((size_ - count_) > buf_size_) {
	    		file_.read (memblock_, buf_size_);
	    		count_ += buf_size_;
	    	}
	    	else {
	    		file_.read (memblock_, size_ - count_);
	    		count_ = size_;
	    	}
	    }
	    return false;
	}
}
