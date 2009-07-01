#ifndef DOC_FREQ_H_
#define DOC_FREQ_H_


///may use hash map later for more efficiency
///#include <tr1/unordered_map>

#include <map>
#include <string>
#include <vector>
//#include <utils/icstring.hpp>

#include <stpl/characters/stpl_unicode.h>
#include <stpl/characters/stpl_chinese.h>
//#include <stpl/misc/stpl_unidoc.h>

#include "word.h"
#include "freq.h"
#include "doc.h"

/**
 */
class FreqCounter {

public:
	typedef Freq::string_type						string_type;
	typedef Freq::word_type 						word_type;
	typedef Freq::word_ptr_type						word_ptr_type;
	typedef Freq::freq_type						 	freq_type;
	typedef Freq::array_type						array_type;
	typedef Freq::array_array_type					array_array_type;

	//typedef stpl::UNICODE::DefaultChineseParser::document_type::entity_iterator	entity_iterator;
	typedef Doc::entity_iterator					entity_iterator;


private:
	string_type stream_;
	Freq*		freq_;
	array_type	wa_;
	//int			len_; /// the length of whole unsegmented document

public:
	FreqCounter(Freq* freq)
		: freq_(freq) {}

	/**
	 * @param stream The content character stream
	 * @param freq The map for the frequcecies of words
	 */
	FreqCounter(string_type stream, Freq* freq)
			: stream_(stream), freq_(freq) {}
	~FreqCounter() {}

	void count(int max, int min = 1);
	void count() { count(stream_.length(), 1); }
	void count(Doc& doc, int max, int min);
	string_type& stream() { return stream_; }
	void stream(string_type& str) { stream_ = str; }

	void show_array();
	void assign_array();
	void remove_array_r();  // remove the redundancy
	void compat_array(array_type& wa);

	array_type& wa() { return wa_; }
	const array_type& wa() const { return wa_; }

	void overall(Freq& freq);

private:
	void add_word(entity_iterator begin, entity_iterator end, int max, int min);
};

#endif /*DOC_FREQ_H_*/
