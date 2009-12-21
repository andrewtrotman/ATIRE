#ifndef DOC_FREQ_H_
#define DOC_FREQ_H_


///may use hash map later for more efficiency
///#include <tr1/unordered_map>

#include <map>
#include <string>
#include <vector>

#include "freq.h"
#include "uniseg_types.h"

/**
 */
class FreqCounter {

private:
	string_type stream_;
	Freq*		freq_;
	array_type	wa_;
	int			languages_;
	//int			len_; /// the length of whole unsegmented document

public:
	FreqCounter(Freq* freq, uniseg_encoding::language lang = uniseg_encoding::CHINESE)
		: freq_(freq), languages_(lang) {}

	/**
	 * @param stream The content character stream
	 * @param freq The map for the frequcecies of words
	 */
	FreqCounter(string_type stream, Freq* freq, uniseg_encoding::language lang = uniseg_encoding::CHINESE)
			: stream_(stream), freq_(freq), languages_(lang) {}
	~FreqCounter() {}

	void count(int max, int min = 1);
	void count() { count(get_utf8_string_length(stream_), 1); }
	string_type& stream() { return stream_; }
	void stream(string_type& str) { stream_ = str; }

	void show_array();
	void assign_array();
	void remove_array_r();  // remove the redundancy
	void compat_array(array_type& wa);

	array_type& wa() { return wa_; }
	const array_type& wa() const { return wa_; }

	void overall(Freq& freq);

	void setup_language(UNISEG_encoding::language lang) { languages_ |= lang; }

private:
	void add_word(const char *begin, const char *end, int max, int min);
	void count(const char *begin, const char *end, int max, int min);
	void count_ones(Freq& freq, const char *begin, const char *end);
};

#endif /*DOC_FREQ_H_*/
