#ifndef QTYPE_H_
#define QTYPE_H_

#include "word.h"
#include <map>

typedef SEGMENTATION::Word::string_type				string_type;
typedef SEGMENTATION::Word::word_type				word_type;
typedef SEGMENTATION::Word::word_ptr_type			word_ptr_type;
typedef const word_ptr_type							const_word_ptr;

typedef std::map<string_type, word_ptr_type>		freq_type;
typedef std::map<string_type, word_ptr_type>		word_map;

typedef std::vector<word_ptr_type>					array_type;
typedef std::vector<array_type >					array_array_type;

#endif
